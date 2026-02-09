#!/bin/bash

# Change the working directory to this one.
# This makes sure one can run this script from a different directory.
# script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# cd "$script_dir" || exit 1


## UTILS is called from within colors, this will be the home directory for this script


#########################
###### DIRECTORIES ######
#########################

CACHE_FILE="graph_counts.json"

pattern="nauty"
nauty_path=$(find ../../.. -maxdepth 2 -type d -name "*${pattern}*" | head -n 1)
if [[ -z "$nauty_path" ]]; then
	echo >&2 "Error: Nauty not found."
	exit 1
fi

pattern="plantri"
plantri_path=$(find ../../.. -maxdepth 2 -type d -name "*${pattern}*" | head -n 1)
if [[ -z "$plantri_path" ]]; then
	echo >&2 "Error: Plantri not found."
	exit 1
fi

#######################
###### FUNCTIONS ######
#######################

usage() {
  echo "Error: Invalid usage." >&2
  echo "Check README for help." >&2
  exit 1
}

# Function that generates the plantri output in a range of vertices
gen_range_graphs() {
	if [[ "$raw" == 0 ]]; then
		# Only if we are not in raw mode do we print this.
		echo "Generating graphs from $1 to $2 vertices." >&2 # We print to stderr, so this isn't on stdout
	fi
	res="$3"
	res=$(( res > 0 ? res : 0 ))
	for num in $(seq "$1" "$2"); do
    "./$plantri_path/plantri" "$plantri_options" "$num" "$res/$number_of_processes" 2>/dev/null
	done
}

show_help() {
	cat <<EOF
  Usage:
    ${0##*/} NUMBER_OF_VERTICES [OPTIONS]

  Description:
    This script helps the user in running the coloring algorithm provided in Main.java.
    The user of this script can choose to use multiple provided options.

    Full explanation of the entire repository can be found in the README.

  Author:
    Arne Claerhout
EOF
}

fetch_optional_arg() {
  local current_ind=$1
  local default_val=$2
  shift 2

    eval nextopt=\${$current_ind}

  if [[ -n $nextopt && $nextopt != -* ]]; then
    echo "$nextopt"
    return 0
  else
    echo "$default_val"
    return 1
  fi
}

# Helper function to get/set graph counts in JSON
# Disclaimer: made with the help of AI
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
		count=$(gen_range_graphs "$n" "$n" 0 2>/dev/null | "./$nauty_path/countg" 2>/dev/null | grep "graphs altogether" | awk '{print $1}')

		# Save it to the JSON file
		local tmp
		tmp=$(mktemp)
		jq ".${mode}.\"${n}\" = $count" "$CACHE_FILE" >"$tmp" && mv "$tmp" "$CACHE_FILE"
		echo "$count"
	fi
}

read_stdin() {
	while IFS= read -r line; do
		echo "$line"
	done
}

###################
#### VARIABLES ####
###################


coloring=""
filter=0
min_chrom=0
manual=""
raw=0
overview=false
show=""
coloring_method=0
startn=-1
endn=-1
method=0
check_condition=0
mode="triangulation"
number_of_processes=1
plantri_options="-g"
output_path=outputs/$(date +"%F-%H-%M-%S").txt

############################
#### NUMBER OF VERTICES ####
############################

if [[ "$1" =~ ^[0-9]+(:[0-9]+)?$ ]]; then
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
  OPTIND=$((OPTIND + 1))
else
  echo "Error: Number of vertices not set." >&2
  exit 1
fi

#################
#### OPTIONS ####
#################

while getopts ":hCcm:f:porsPLBaxM:" opt; do
  case $opt in
    h)
  		show_help
  		exit 0
  		;;
    C)
      javac graphs/*.java
      echo "Code compiled." >&2
      ;;
    c)
      coloring=$(fetch_optional_arg "$OPTIND" "proper" "$@")
      if [[ $? -eq 0 ]]; then # Skip one index if an option was found
        OPTIND=$((OPTIND + 1))
      fi
      ;;
    m)
      manual=$OPTARG
      ;;
    f)
      filter=$OPTARG
      ;;
    p)
      num_graphs=0
      ;;
    o)
      overview=true
      ;;
    r)
      raw=$(fetch_optional_arg "$OPTIND" 1 "$@")
      if [[ "$raw" -lt 0 || "$raw" -gt 4 ]]; then
      	# We only want certain raw options
      	raw=1
      fi
      if [[ $? -eq 0 ]]; then # Skip one index if an option was found
        OPTIND=$((OPTIND + 1))
      fi
      ;;
    s)
      show=$(fetch_optional_arg "$OPTIND" "png" "$@")
      if [[ $? -eq 0 ]]; then # Skip one index if an option was found
        OPTIND=$((OPTIND + 1))
      fi
      ;;
    P)
      method=1
      ;;
    L)
      method=2
      ;;
    B)
      method=3
      ;;
    a)
      mode="allplanar"
      plantri_options="-gpc1m1"
      ;;
    x)
      check_condition=$(fetch_optional_arg "$OPTIND" 1 "$@")
      if [[ $? -eq 0 ]]; then # Skip one index if an option was found
        OPTIND=$((OPTIND + 1))
      fi
      ;;
    M)
      number_of_processes=$OPTARG
      if [[ $number_of_processes -lt 1 ]]; then
        echo "Error: Negative number of processes given" >&2
        exit 1
      fi
      ;;
    \?)
      exit 3 #invalid option
      ;;
  esac
done

### SHOW-CHECK
if [[ "$show" != "" ]]; then
  # If show is chosen, we always pick these values
	raw=3
	overview=false
	unset num_graphs
fi

### FILTER CHECK
if [[ "$filter" =~ ^-?[0-9]+$ ]]; then
	# Filter is a number
	min_chrom="$filter"
fi


### GRAPH COUNTING
if [[ -v num_graphs ]]; then
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