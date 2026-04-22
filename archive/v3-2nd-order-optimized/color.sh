#!/bin/bash

# Change the working directory to this one.
# This makes sure one can run this script from a different directory.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1

source ./scripts/utils.sh

# Function that generates the plantri output in a range of vertices
gen_range_graphs() {
	if [[ "$raw" == 0 ]]; then
		# Only if we are not in raw mode do we print this.
		echo "Generating graphs from $1 to $2 vertices." >&2 # We print to stderr, so this isn't on stdout
	fi
	for num in $(seq "$1" "$2"); do
		if [[ "$mode" == "allplanar" ]]; then
			echo "test" >&2
			"./$plantri_path/plantri" "-gpc1m1" "$num" 2>/dev/null | "./$nauty_path/shortg" -q 2>/dev/null
		else
			"./$plantri_path/plantri" -g "$num" 2>/dev/null # We get rid of the extra printing to the terminal
		fi
	done
}

show_func() {
	if [[ "$show" != "" ]]; then
		# Check if venv has been created
		if [ ! -d "venv" ]; then
			echo "Error: venv hasn't been created yet."
			exit 1
		fi
		source venv/bin/activate
		mkdir -p images
		# We also remove all previous files from the directory (-f ignores no file error)
		rm -f images/*

		# Run Python script with stdin
		"venv/bin/python" scripts/graph6_to_image.py "$show" </dev/stdin
	else
		cat
	fi
}

write_to_file() {
	if [[ "$overview" == false && ("$manual" == false || "$manual" == pipe) ]]; then
		mkdir -p outputs
		tee "outputs/$(date +"%F-%H-%M-%S").txt"
	else
		cat
	fi
}

java_alg() {
	if [[ -v num_graphs ]]; then
		pv -l "-s $num_graphs" | java graphs.Main "$coloring" "$overview" "$raw" "$min_chrom" "$method" "$check_condition"
	else
		java graphs.Main "$coloring" "$overview" "$raw" "$min_chrom" "$method" "$check_condition"
	fi
}

choose_incoming_graphs() {
	# First we check whether we have a filter file
	if ! [[ "$filter" =~ ^-?[0-9]+$ ]]; then
		# The filter is not an integer
		# We parse the given filter
		# Check if venv has been created
		if [ ! -d "venv" ]; then
			echo "Error: venv hasn't been created yet."
			exit 1
		fi
		source venv/bin/activate

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
			if [[ "$overview" != True && "$overview" != False ]]; then
				# overview isn't a boolean, we update the arguments
				change="$overview"
				if [[ "$raw" != 0 ]]; then
					changeoverview=true
				fi
			fi

			gen_range_graphs "$n" "$n" "$raw" | eval "\"./$nauty_path/pickg\" $rest" 2>/dev/null
		done

		# We deactivate the venv
		deactivate
	elif [[ -z "$manual" ]]; then
		# Not manual
		gen_range_graphs "$startn" "$endn" "$raw"
	elif [[ "$manual" == "pipe" ]]; then
		# Own stream chosen
		read_stdin
	else
		echo "$manual"
	fi

}

### EXECUTION

if [[ "$overview" == true && "$coloring" == all ]]; then
	declare -a colorings=("proper" "odd" "pUMo" "pUMc" "pCFo" "pCFc" "iUMo" "iUMc" "iCFo" "iCFc")
	for i in "${colorings[@]}"; do
		coloring="$i"
		choose_incoming_graphs | java_alg | write_to_file | show_func
	done
else
	choose_incoming_graphs | java_alg | write_to_file | show_func
fi

# Extra output for show functionality to signal the end of the generation.
if [[ "$show" != "" ]]; then
	echo "Generated all images."
fi
