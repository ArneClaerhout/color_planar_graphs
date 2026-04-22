#!/bin/bash


# Change the working directory to this one.
# This makes sure one can run this script from a different directory.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1

source ./scripts/utils.sh

activate_venv() {
  # Check if venv has been created
  if [ ! -d "venv" ]; then
    echo "Error: venv hasn't been created yet."
    exit 1
  fi
  source venv/bin/activate
}

show_func() {
	if [[ "$show" != "" ]]; then
		mkdir -p images
		# We also remove all previous files from the directory (-f ignores no file error)
		rm -f images/*

    activate_venv

		# Run Python script with stdin
		"venv/bin/python" scripts/graph6ToImage.py "images/" "$show" </dev/stdin
	else
		cat
	fi
}

write_to_file() {
	if [[ $number_of_processes -ne 1 || ("$overview" == false && ("$manual" == false || "$manual" == pipe)) ]]; then
	  mkdir -p "outputs"

	  path="$output_path"
    if [[ $number_of_processes -ne 1 && "$1" -ne -1 ]]; then
      path="outputs/process_$1.txt"
      cat > "$path"
    elif [[ $number_of_processes -ne 1 && "$overview" == true ]]; then
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

  change=""
  changeoverview=false
  "venv/bin/python" scripts/parseFilter.py "$filter" | while read -r line; do
    # We first parse the line
    # This changes all the variables to the ones given in the filter
    read -r raw overview n coloring minChrom rest <<<"$line"
    if [[ "$change" != "" ]]; then

      # Last cycle we got a filter name
      echo ":raw $raw"
      echo ":overview $overview"
      echo ":coloring $coloring"
      echo ":minChrom $minChrom"
      if [[ "$changeoverview" == true ]]; then
        echo ":update overview"
        changeoverview=false
      fi
      if [[ "$raw" == 0 ]]; then
        echo ":print $change"
      fi
      change=""
    fi
    if [[ "$overview" != true && "$overview" != false ]]; then
      # overview isn't a boolean, we update the arguments
      change="$overview"
      if [[ "$raw" != 0 ]]; then
        changeoverview=true
      fi
    fi

    gen_range_graphs "$n" "$n" "$raw" 0 | eval "\"./$nauty_path/pickg\" $rest" 2>/dev/null
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


java_alg() {
	if [[ -v num_graphs ]]; then
		pv -l "-s $(( num_graphs / number_of_processes ))" | java graphs.Main "$coloring" "$overview" "$raw" "$min_chrom" "$method" "$check_condition"
	else
		java graphs.Main "$coloring" "$overview" "$raw" "$min_chrom" "$method" "$check_condition"
	fi
}

choose_incoming_graphs() {
	# First we check whether we have a filter file
	if ! [[ "$filter" =~ ^-?[0-9]+$ ]]; then
	  exit_on_multiprocessing
		filter_file_parse
	elif [[ "$manual" == "" ]]; then
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
}

combine_outputs_M() {
  for i in $(seq 0 $(("$number_of_processes" - 1))); do
    path="outputs/process_$i.txt"
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
  choose_incoming_graphs "$1" | java_alg | write_to_file "$1"
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
if [[ $number_of_processes -eq 1 ]]; then
  execute | show_func
else
  execute_M 0 &
  for i in $(seq 1 $(("$number_of_processes" - 1)));
  do
    execute_M "$i" 2>/dev/null &
  done
  wait
  combine_outputs_M | write_to_file -1 | show_func
  
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
