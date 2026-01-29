#!/bin/bash

# Change the working directory to this one.
# This makes sure one can run this script from a different directory.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1

##################
###### HELP ######
##################

show_help() {
	cat <<EOF
  Usage:
    ${0##*/} NUMBER_OF_VERTICES [OPTIONS]

  Description:
    This script helps the user in running the coloring algorithm provided in Main.java.
    The user of this script can choose to use multiple provided options.

    Full explanation of the entire repository can be found here:
        https://github.com/ArneClaerhout/Bachelorproef

  Positional Arguments:
    NUMBER_OF_VERTICES
        The amount of vertices the to be colored graphs should have.
        This can either be a range, or just a single number.
        The range should be given in the following format:

        MIN_VERTICES:MAX_VERTICES

  Options:
    -h, --help
        Show this help message and exit.

    -c, --coloring
        The chosen coloring to use.
        The default coloring is the proper coloring.
        Examples: proper, odd, iUMo, pCFc, ...

    -m, --manual GRAPH
        Whether the user wants to manually input graphs.
        The value for this option should be the graph to color in graph6 format.
        If the user would like to input the graphs through a pipe,
        the GRAPH value should be "pipe".

	-a, --all
		Instead of only generating planar triangulations,
		this option goes through all planar graphs instead.

    -f, --filter MIN_COLORS
        Adds a filter to the outputted graphs.
        MIN_COLORS is the minimum amount of colors needed before the graph gets to be output to the terminal.
        When the chromatic number of a graph is strictly smaller than this value, it gets skipped.

    -o, --overview
        Gives an overview of all the outputted graphs instead of
        actually printing all graphs and their corresponding chromatic numbers.
        This also keeps track of the needed time for the algorithm as a whole.

    -r, --raw VALUE
        Alternates the output of the algorithm to a more raw output.
        VALUE should be a value equal to 1, 2 or 3.
          - A value of 1 makes sure the output is only the chromatic numbers of the graphs.
          - A value of 2 only outputs the graph6 strings of the graphs (only useful when filtering).
          - A value of 3 only outputs the graph6 strings followed by the used colors ordered by index.

    -s, --show FORMAT
        Outputs an image of the outputted graphs to the directory images/ .
        The file extension of these images should be given by FORMAT.
        The default value for FORMAT is svg.

  Examples:
    Run the script with a file of graph6 strings:
        cat input.txt | ./${0##*/} --manual pipe

    Run the script with common options:
        ./${0##*/} 3:10 -c pUMo -f 6 --show

    Other examples can be found on the Github page.

  Notes:
    - Arguments can be combined with both short and long forms. The order of the arguments doesn't matter either.

  Author:
    Arne Claerhout
EOF
}

#########################
###### DIRECTORIES ######
#########################

CACHE_FILE="graph_counts.json"

pattern="nauty"
nauty_path=$(find ../.. -maxdepth 1 -type d -name "*${pattern}*" | head -n 1)
if [[ -z "$nauty_path" ]]; then
	echo >&2 "Error: Nauty not found."
	exit 1
fi

pattern="plantri"
plantri_path=$(find ../.. -maxdepth 1 -type d -name "*${pattern}*" | head -n 1)
if [[ -z "$plantri_path" ]]; then
	echo >&2 "Error: Plantri not found."
	exit 1
fi

#######################
###### FUNCTIONS ######
#######################

# Helper function to get/set graph counts in JSON
get_cached_count() {
	local n=$1

	if [[ ! -f "$CACHE_FILE" ]]; then
		echo "{}" >"$CACHE_FILE"
	fi

	# Try to read the value from JSON
	local count
	count=$(jq -r ".${mode}.\"${n}\" // empty" "$CACHE_FILE")

	if [[ -n "$count" ]]; then
		echo "$count"
	else
		# gen_range_graphs takes care of the mode
		count=$(gen_range_graphs "$n" "$n" 2>/dev/null | "./$nauty_path/countg" 2>/dev/null | grep "graphs altogether" | awk '{print $1}')

		# Save it to the JSON file
		local tmp
		tmp=$(mktemp)
		jq ".${mode}.\"${n}\" = $count" "$CACHE_FILE" >"$tmp" && mv "$tmp" "$CACHE_FILE"
		echo "$count"
	fi
}

# Function to parse options with optional arguments
parse_optional_arg() {
	local actualopt1="$1"
	local actualopt2="$2"
	local givenopt="$3"
	local givenarg="$4"
	local default="$5"
	local args=("${@:6}") # Remaining arguments

	# Glued form: -p2
	if [[ "$givenopt" =~ ^$actualopt1(.+) || "$givenopt" =~ ^$actualopt2(.+) ]]; then
		echo "${BASH_REMATCH[1]}"
		return 1
	fi

	# Space-separated: -p 2
	if [[ -n "$givenarg" && "$givenarg" != -* ]]; then
		echo "$givenarg"
		return 2
	fi

	# No argument: just -p
	echo "$default"
	return 1
}

# Function to parse options with mandatory arguments
parse_mandatory_arg() {
	local actualopt1="$1"
	local actualopt2="$2"
	local givenopt="$3"
	local givenarg="$4"

	# Glued form: -fFILE
	if [[ "$givenopt" =~ ^$actualopt1(.+) || "$givenopt" =~ ^$actualopt2(.+) ]]; then
		echo "${BASH_REMATCH[1]}"
		return 1
	fi

	# Space-separated: -f file
	if [[ -n "$givenarg" && "$givenarg" != -* ]]; then
		echo "$givenarg"
		return 2
	fi

	# Missing mandatory argument
	echo "Error: Option '$givenopt' requires an argument." >&2
	exit 1
}

# Function that generates the plantri output in a range of vertices
gen_range_graphs() {
	local s="$1"
	local e="$2"
	shift 2
	if [[ "$raw" == 0 ]]; then
		# Only if we are not in raw mode do we print this.
		echo "Generating graphs from $s to $e vertices." >&2 # We print to stderr, so this isn't on stdout
	fi
	for num in $(seq "$s" "$e"); do
		echo "test: $@" >&2
		"./$plantri_path/plantri" "$@" "$num" 2>/dev/null | "./$nauty_path/shortg" -q 2>/dev/null
	done
}

read_stdin() {
	while IFS= read -r line; do
		echo "$line"
	done
}

show_func() {
	if [[ "$show" == true ]]; then
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
		"venv/bin/python" graph6_to_image.py "$show_value" </dev/stdin
	else
		cat
	fi
}

write_to_file() {
	if [[ "$overview" == false && ("$manual" == "" || "$manual" == pipe) ]]; then
		mkdir -p outputs
		tee "outputs/$(date +"%F-%H-%M-%S").txt"
	else
		cat
	fi
}

java_alg() {
	if [[ "$progressview" == true ]]; then
		pv -l "-s $num_graphs" | java Main "$coloring" "$overview" "$raw" "$minChrom" "$method" "$check_condition"
	else
		java Main "$coloring" "$overview" "$raw" "$minChrom" "$method" "$check_condition"
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
		"venv/bin/python" parseFilter.py "$filter" | while read -r line; do
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
		echo "$startn" "$endn" "$@" >&2
		gen_range_graphs "$startn" "$endn" "$@"
	elif [[ "$manual" == "pipe" ]]; then
		# Own stream chosen
		read_stdin
	else
		echo "$manual"
	fi

}

multithreading() {
	base_options="$@"
	echo "$base_options" >&2
	if ! [[ "$filter" =~ ^-?[0-9]+$ ]]; then
		echo "Error: Multithreading with a complex filter is not yet supported." >&2
	elif [[ -z "$manual" ]]; then
		if [[ "$threads" == 1 ]]; then
			if [[ "$overview" == true && "$coloring" == all ]]; then
				colorings=("proper" "odd" "pUMo" "pUMc" "pCFo" "pCFc" "iUMo" "iUMc" "iCFo" "iCFc")
				for i in "${colorings[@]}"; do
					coloring="$i"
					choose_incoming_graphs "${base_options[@]}" | java_alg | write_to_file | show_func
				done
			else
				echo "${base_options[@]}" >&2
				choose_incoming_graphs "${base_options[@]}" | java_alg | write_to_file | show_func
			fi

		else
			echo "Multithreading..." >&2
			list_of_threads=$(seq "2" "$threads")
			files=()

			plantri_options=("${base_options[@]}" "1/$threads")
			files+=("outputs/output_1.txt")
			choose_incoming_graphs "${plantri_options[@]}" >"outputs/output_1.txt" &

			for opt in $list_of_threads; do
				plantri_options=("${base_options[@]}" "$opt/$threads")
				echo "${plantri_options[@]}" >&2
				files+=("outputs/output_$opt.txt")
				choose_incoming_graphs "${plantri_options[@]}" 2>/dev/null >"outputs/output_$opt.txt" 2>/dev/null &
			done
			wait # Wait for all the threads to finish

			if [[ "$overview" == true ]]; then
				awk '
					# 1. Capture any line that looks like: "count graphs : chrom=X"
					/graphs : chrom=/ {
						# $1 is the count, $4 is the label (e.g., "chrom=2")
						# We use the label as a key in our associative array
						chrom_sums[$4] += $1
					}

					# 2. Capture the summary line: "count graphs altogether; cpu=X"
					/altogether/ {
						total_all += $1
						
						# Extract CPU time: split "cpu=0.06" by "=" to get "0.06"
						split($4, cpu_part, "=")
						total_cpu += cpu_part[2]
					}

					# 3. Once all files are processed, print everything we found
					END {
						print "--- DYNAMIC SUMMARY REPORT ---"
						
						# Loop through the keys we found in the array
						for (label in chrom_sums) {
							printf "%-15s : %d\n", label, chrom_sums[label]
						}
						
						print "------------------------------"
						printf "Total Combined  : %d\n", total_all
						printf "Total CPU Time  : %.2f sec\n", total_cpu
					}
				' "${files[@]}"
			else

				paste -d '\n' "${files[@]}" | write_to_file | show_func

			fi

			# rm "${files[@]}"

		fi
	else
		echo "Error: Multithreading with a manual input is not yet supported." >&2
	fi

}

#######################
###### VARIABLES ######
#######################

# Default values for our flags
coloring=""
filter=0
minChrom=0
manual=""
raw=0
overview=false
progressview=false
num_graphs=0
show=false
show_value="svg"
method=0
startn=-1
endn=-1
check_condition=false
mode="triangulation"
plantri_options=()
plantri_options+="-g"
threads=1

#######################
###### ARGUMENTS ######
#######################

# Loop over all flags and their values
while [[ $# -gt 0 ]]; do
	case $1 in
	-h | --help)
		show_help
		exit 0
		;;
	-c | --coloring)
		coloring=$(parse_mandatory_arg "-c" "--coloring" "$1" "$2")
		shift $?
		;;
	-m | --manual)
		manual=$(parse_mandatory_arg "-m" "--manual" "$1" "$2")
		shift $?
		;;
	-f | --filter)
		filter=$(parse_mandatory_arg "-f" "--filter" "$1" "$2")
		shift $?
		;;
	-pv | --progressview)
		progressview=true
		shift 1
		;;
	-o | --overview)
		overview=true
		shift
		;;
	-r* | --raw*)
		raw=$(parse_optional_arg -r --raw "$1" "$2" 1)
		shift $?
		;;
	-s* | --show*)
		show=true
		show_value=$(parse_optional_arg -s --show "$1" "$2" "svg")
		shift $?
		;;
	-pq)
		method=1
		shift 1
		;;
	-ll)
		method=2
		shift 1
		;;
	-bs)
		method=3
		shift 1
		;;
	-a | --all)
		mode=allplanar
		echo "test" >&2
		plantri_options+=("-pc1m1")
		shift 1
		;;
	-x | --condition)
		check_condition=true
		shift 1
		;;
	-C | --Compile)
		javac Main.java
		if [[ "$raw" == 0 ]]; then
			# Only if we are not in raw mode do we print this.
			echo "Code compiled." >&2
		fi
		shift 1
		;;
	-M*)
		threads=$(parse_optional_arg -M -M "$1" "$2" 4)
		shift $?
		threads=$(($threads > 1 ? $threads : 1))
		;;
	-* | --*)
		echo "Error: Unknown option $1" >&2
		exit 1
		;;
	*)
		if [[ "$startn" == -1 && "$endn" == -1 ]]; then
			# Number of vertices
			# We extract them
			if [[ "$1" == *:* ]]; then
				# Split the argument into start and end
				IFS=':' read -r startn endn <<<"$1"
			else
				# Only one value given; use it as both start and end
				startn="$1"
				endn="$1"
			fi
			shift 1
		elif ! [[ "$1" =~ ^-?[0-9]+$ ]]; then
			echo "Error: Argument given when none should be given" >&2
			exit 1
		else
			echo "Error: Number of vertices already set." >&2
			exit 1
		fi
		;;
	esac
done

#######################
###### ALGORTIHM ######
#######################

# We make sure to recompile the code
# javac Main.java
#
#### RAW-CHECK
if [[ "$raw" -ne 1 && "$raw" -ne 2 && "$raw" -ne 3 && "$raw" -ne 4 && "$raw" -ne 0 ]]; then
	# We only want three raw options
	raw=1
fi

### SHOW-CHECK
if [[ "$show" == true ]]; then
	# If show is chosen, we always pick these values
	raw=3
	overview=false
	progressview=false
fi

### FILTER CHECK
if [[ "$filter" =~ ^-?[0-9]+$ ]]; then
	# Filter is a number
	minChrom="$filter"
fi

### NUMBER OF VERTICES CHECK
if [[ -z "$startn" && -z "$manual" && "$filter" =~ ^-?[0-9]+$ ]]; then
	# There is no n given and we don't want to manually give a graph
	# And we also haven't recieved a filter file (as it is just a number) -> ERROR
	# We only check startn, as both are either filled or not filled
	echo "Error: Number of vertices wasn't given." >&2
	exit 1
fi

### GRAPH COUNTING
if [[ "$progressview" == true ]]; then
	echo "Calculating or retrieving graph counts." >&2

	# Ensure jq is installed
	if ! command -v jq &>/dev/null; then
		echo "Error: 'jq' is required for caching. Install it with 'sudo apt install jq'." >&2
		exit 1
	fi

	total_sum=0

	# Calculate range and use the caching function
	if [[ -n "$manual" ]]; then
		num_graphs=0 # Manual/Pipe input cannot be pre-counted
	else
		for ((n = startn; n <= endn; n++)); do
			# Use the get_cached_count function logic here
			count=$(get_cached_count "$n")
			total_sum=$((total_sum + count))
		done
		num_graphs=$total_sum
	fi
fi

### EXECUTION
multithreading "${plantri_options[@]}"

# Extra output for show functionality to signal the end of the generation.
if [[ "$show" == true ]]; then
	echo "Generated all images."
fi
