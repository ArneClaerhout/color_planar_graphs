#!/bin/bash

# Change the working directory to this one.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit
## This above line moves the working directory to the v5-C directory.

numvertices="$1"
shift 1
method=""

args=("$numvertices")
argssimple=("$numvertices")

# Loop over all flags and their values
while [[ $# -gt 0 ]]; do
	case $1 in
	-c)
		args+=(-c "$2")
		argssimple+=(-c "$2")
		shift 2
		;;
	-pq)
		args+=(-pq)
		shift 1
		;;
	-ll)
		args+=(-ll)
		shift 1
		;;
	-bs)
		args+=(-bs)
		shift 1
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
			echo "  Correct: $line1"
			echo "  Incorrect: $line2"
			# This variable assignment now persists
			diff=true
		fi
		;;
	*) ;;

	esac
	# We pipe these outputs, this will make sure that even if the length is incorrect, it will still output
done < <(paste <(./../../v4-multiprocessing/color.sh "${argssimple[@]}" 2>/dev/null) \
	<(./../color.sh "${args[@]}" 2>/dev/null))

if [[ "$diff" == true ]]; then
	echo "Outputs differ."
else
	echo "Outputs match, optimized algorithm is correct."
fi
