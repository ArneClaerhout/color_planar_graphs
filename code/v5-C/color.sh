#!/bin/bash


# Change the working directory to this one.
# This makes sure one can run this script from a different directory.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1

source ./scripts/utils.sh

get_view_cmd() {
  if [[ "$1" -gt 0 ]]; then
    echo "pv -l -s $(( num_graphs / number_of_processes ))"
  else
    echo "" # No command needed
  fi
}

write_to_file() {
  if [[ "$number_of_processes" -ne 1 || ("$overview" == false && ("$manual" == "" || "$manual" == pipe)) ]]; then
    mkdir -p "outputs"
    local path="$output_path"

    if [[ "$number_of_processes" -ne 1 && "$1" -ne -1 ]]; then
      path="outputs/process_$((offset + $1)).txt"
      cat > "$path"
    elif [[ "$number_of_processes" -ne 1 && "$overview" == true ]]; then
      cat > "$path"
    else
      tee "$path"
    fi
  else
    cat
  fi
}

filter_file_parse() {
  # The filter is not an integer
  # We parse the given filter
  # Check if venv has been created
  activate_venv

  "venv/bin/python" scripts/parseFilter.py "$filter" | while read -r line; do
    # We first parse the line
    read -r n rest <<<"$line"

#    echo "$n" >&2

    # Then we filter the output using nauty
    gen_range_graphs "$n" "$n" "$1" | "./$nauty_path/pickg" "$rest" 2>/dev/null
  done

  # We deactivate the venv
  deactivate
}

exit_on_multiprocessing() {
  # This is called when multiprocessing isn't allowed and should be exited
  if [[ $number_of_processes -ne 1 ]]; then
    echo "Error: Multiprocessing is not allowed in this context." >&2
    exit 1
  fi
}


choose_incoming_graphs() {
	# First we check whether we have a filter file
	if [[ "$filter" != 0 ]]; then

	  # When both a filter- and input file are given, ignore one of the two
	  if [[ "$file_name" != "" ]]; then
	    echo >&2 "Ignoring the given input file and using the filter file instead."
	  fi
		filter_file_parse "$1"

	# We have an input file to read
	elif [[ "$file_name" != "" ]]; then

    echo >&2 "Reading the input file."
	  # We read the file as a different process
	  exec < "$file_name" && cat

	# We don't have a filter file
	else

	  # Not manual, this is the normal path to take
    if [[ "$manual" == "" ]]; then
      gen_range_graphs "$startn" "$endn" "$1"

    # There is a piped input to use
    elif [[ "$manual" == "pipe" ]]; then
      exit_on_multiprocessing
      cat

    # There is one single manual input graph
    else
      exit_on_multiprocessing
      echo "$manual"
    fi
	fi
}

show_func() {
  if [[ "$1" != "" ]]; then
  	./scripts/showGraph6.sh "$1"
  else
    cat
  fi
}

combine_outputs_M() {
  for i in $(seq 0 $(("$number_of_processes" - 1))); do
    path="outputs/process_$((offset + i)).txt"
    if [[ "$overview" == true ]]; then
      echo "Process $(( i + 1 )):"
    fi
    cat "$path"
    rm "$path"
  done
}

combine_overviews_M() {
  activate_venv
  "venv/bin/python" scripts/parseMultiOutput.py "$output_path"
  deactivate
}

c_alg() {
  ./graphs/build "$coloring" "$overview" "$raw" "$min_chrom" "$method" "$check_condition"
}

execute_M() {
  local proc_id="$1"
  local show_pv="$2"

  # Determine the view command
  local view_cmd=$(get_view_cmd "$show_pv")

  # If view_cmd is empty, the data flows directly to c_alg, it doesn't create a pv process.
  if [[ -n "$view_cmd" ]]; then
    choose_incoming_graphs "$proc_id" | $view_cmd | c_alg | write_to_file "$proc_id"
  else
    choose_incoming_graphs "$proc_id" | c_alg | write_to_file "$proc_id"
  fi
}

execute() {
  # Execute with or without progress view
  if [[ "$overview" == true && "$coloring" == all ]]; then
    declare -a colorings=("proper" "odd" "pUMo" "pUMc" "pCFo" "pCFc" "iUMo" "iUMc" "iCFo" "iCFc")
    for i in "${colorings[@]}"; do
      coloring="$i"
      execute_M -1 "$num_graphs"
    done
  else
    execute_M -1 "$num_graphs"
  fi
}

###             ###
###  Execution  ###
###             ###

# We ignore the multithreading when an input file is given
if [[ "$number_of_processes" -eq 1 || "$file_name" != "" ]]; then

  execute | show_func "$show"

else
  # We split up the execution, only using pv on the first process
  execute_M 0 "$num_graphs" &

  # Create all other processes
  for i in $(seq 1 $(("$number_of_processes" - 1)));
  do
    execute_M "$i" 0 2>/dev/null &
  done

  # Wait for all processes to finish
  wait

  # Combine the outputs and write + show graphs
  combine_outputs_M | write_to_file -1 | show_func "$show"

  # When overview is required, we use a different specialised python script
  if [[ "$overview" == true ]]; then
    # We first combine the overviews and then read the output
    combine_overviews_M
    cat "$output_path"
    rm "$output_path"
  fi

fi

###         ###
###  Other  ###
###         ###

# Extra output for show functionality to signal the end of the generation.
if [[ "$show" != "" ]]; then
	echo "Generated all images."
fi


# When we're profiling, the gcda files need to be moved
if [[ "$profiling" == "true" ]]; then
  gcov graphs/build-vertex.gcno
  gcov graphs/build-graph.gcno
  mv graph.c.gcov graphs
  mv vertex.c.gcov graphs
  rm -f types.h.gcov
fi
