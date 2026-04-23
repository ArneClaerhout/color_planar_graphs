#!/bin/bash

# Change the working directory to this one.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1

pattern="plantri"
plantri_path=$(find ../.. -maxdepth 1 -type d -name "*${pattern}*" | head -n 1)
if [[ -z "$plantri_path" ]]; then
  >&2 echo "Error: Plantri not found."
  exit 1
fi


# Function that generates the plantri output in a range of vertices
gen_range_graphs() {
	if [[ "$raw" == false ]]; then
		# Only if we are not in raw mode do we print this.
		echo "Generating graphs from $1 to $2 vertices." >&2 # We print to stderr, so this isn't on stdout
	fi
	for num in $(seq "$1" "$2"); do
		"./$plantri_path/plantri" -g "$num" 2>/dev/null # We get rid of the extra printing to the terminal
	done
}

coloring="proper"
numvertices="$1"
shift 1

# Loop over all flags and their values
while [[ $# -gt 0 ]]; do
	case $1 in
	-c | --coloring)
		coloring="$2"
		shift 2
		;;
	-* | --*)
		echo "Unknown option $1"
		exit 1
		;;
	*)
		# End of the flags
		break
		;;
	esac
done

echo "Starting comparison."

# First argument for this script is the amount of vertices
diff=false
count=0

while IFS=$'\t' read -r line1 line2; do
	if [[ $((count % 10000)) == 0 && "$count" != 0 ]]; then
		echo "count: $count"
	fi
	((count = count + 1))
	case "$line1" in
	*:*)
		if [[ "$line1" != "$line2" && "$line1" != *"coloring"* && "$line1" != *"time"* ]]; then
			echo "Difference:"
			echo "  Naive: $line1"
			echo "  Optimized: $line2"
			# This variable assignment now persists
			diff=true
		fi
		;;
	*) ;;

	esac
	# We pipe these outputs, this will make sure that even if the length is incorrect, it will still output
done < <(paste <(./../v1-naive/colorScript.sh "$numvertices" -c "$coloring" 2>/dev/null) \
	<(./colorScript.sh "$numvertices" -c "$coloring" 2>/dev/null))

if [[ "$diff" == true ]]; then
	echo "Outputs differ."
else
	echo "Outputs match, optimized algorithm is correct."
fi
