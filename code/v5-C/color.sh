#!/bin/bash


# Change the working directory to this one.
# This makes sure one can run this script from a different directory.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1

source ./scripts/utils.sh

write_to_file() {
	if [[ "$number_of_processes" -ne 1 || ("$overview" == false && ("$manual" == "" || "$manual" == pipe)) ]]; then
	  mkdir -p "outputs"
	  path="$output_path"
    if [[ "$number_of_processes" -ne 1 && "$1" -ne -1 ]]; then
      num_proc="$1"
      path="outputs/process_$((offset + num_proc)).txt"
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


c_alg() {
  if [[ -n "$file_name" ]]; then
    if [[ "$number_of_processes" -eq 1 ]]; then
      pv -l "-s $num_graphs" "$file_name" | ./graphs/build "$coloring" "$overview" "$raw" "$min_chrom" "$method" "$check_condition"
    else
      echo "TODO"
    fi
	elif [[ "$1" -eq 1 ]]; then
		pv -l "-s $(( num_graphs / number_of_processes ))" | ./graphs/build "$coloring" "$overview" "$raw" "$min_chrom" "$method" "$check_condition"
	else
		./graphs/build "$coloring" "$overview" "$raw" "$min_chrom" "$method" "$check_condition"
	fi
}

choose_incoming_graphs() {
	# First we check whether we have a filter file
	if [[ "$filter" != 0 ]]; then
		filter_file_parse "$1"
	elif [[ -z "$file_name" ]]; then
	  # We don't have an input file

    if [[ "$manual" == "" ]]; then
      # Not manual
      gen_range_graphs "$startn" "$endn" "$1"
    elif [[ "$manual" == "pipe" ]]; then
      exit_on_multiprocessing
      # Own stream chosen
      read_stdin
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

execute_M() {
  choose_incoming_graphs "$1" | c_alg "$2" | write_to_file "$1"
}

execute() {
  if [[ "$overview" == true && "$coloring" == all ]]; then
    declare -a colorings=("proper" "odd" "pUMo" "pUMc" "pCFo" "pCFc" "iUMo" "iUMc" "iCFo" "iCFc")
    for i in "${colorings[@]}"; do
      coloring="$i"
      execute_M -1
    done
  else
    execute_M -1
  fi
}

### EXECUTION
if [[ "$number_of_processes" -eq 1 || -n "$file_name" ]]; then
  execute | show_func "$show"
else
  # We check if the number of graphs is set
  [[ -v num_graphs ]] && val=1 || val=0

  # We split up the execution, only using pv on the first process
  execute_M 0 "$val" &
  for i in $(seq 1 $(("$number_of_processes" - 1)));
  do
    execute_M "$i" 0 2>/dev/null &
  done
  wait
  combine_outputs_M | write_to_file -1 | show_func "$show"
  
  if [[ "$overview" == true ]]; then
    # We first combine the overviews and then read the output
    combine_overviews_M
    cat "$output_path"
    rm "$output_path"
  fi

fi



# Extra output for show functionality to signal the end of the generation.
if [[ "$show" != "" ]]; then
	echo "Generated all images."
fi


# If we're profiling, the gcda files need to be moved
if [[ "$profiling" == "true" ]]; then
  gcov graphs/build-vertex.gcno
  gcov graphs/build-graph.gcno
  mv graph.c.gcov graphs
  mv vertex.c.gcov graphs
  rm -f types.h.gcov
fi
