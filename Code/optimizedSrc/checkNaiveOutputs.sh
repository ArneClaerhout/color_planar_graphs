#!/bin/bash

# Change the working directory to this one.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1

# Function that generates the plantri output in a range of vertices
gen_range_graphs() {
  if [[ "$raw" == false ]]; then
    # Only if we are not in raw mode do we print this.
    echo "Generating graphs from $1 to $2 vertices." >&2 # We print to stderr, so this isn't on stdout
  fi
  for num in $(seq "$1" "$2"); do
    ./../../plantri55/plantri -g "$num" 2>/dev/null # We get rid of the extra printing to the terminal
  done
}

coloring="proper"
numvertices="$1"
shift 1


# Loop over all flags and their values
while [[ $# -gt 0 ]]; do
  case $1 in
    -c|--coloring)
      coloring="$2"
      shift 2
      ;;
    -*|--*)
      echo "Unknown option $1"
      exit 1
      ;;
    *)
      # End of the flags
      break
      ;;
  esac
done

# First argument for this script is the amount of vertices


output2=$(./../naiveSrc/colorScript.sh "$numvertices" -c "$coloring" --raw --overview 2>/dev/null)
## We also get rid of the extra printing to the terminal
# We get the time it took to run

rest2=$(echo "$output2" | sed '$d')
time_taken=$(echo "$output2" | tail -1 | grep -oG '[0-9.]* sec')
echo "Naive algorithm done in ${time_taken}onds."

output1=$(./colorScript.sh "$numvertices" -c "$coloring" --raw --overview 2>/dev/null)
rest1=$(echo "$output1" | sed '$d')
time_taken=$(echo "$output1" | tail -1 | grep -oG '[0-9.]* sec')
echo "Optimized algorithm done in ${time_taken}onds."

# The two outputs from nauty and my own program, stripped of spaces and the last line
# We now compare the two (in terms of correctness)
if [[ "$rest1" == "$rest2" ]]; then
  echo "Outputs match, optimized algorithm is correct."
else
  echo "Outputs differ."
fi



