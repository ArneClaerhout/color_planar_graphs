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

echo "Starting comparison."

# First argument for this script is the amount of vertices
diff=false

while IFS= read -r line1 && IFS= read -r line2 <&3; do
  case "$line1" in
    (*:*)
      if [[ "$line1" != "$line2" ]]; then
          echo "Difference:"
          echo "  Naive: $line1"
          echo "  Optimized: $line2"
          diff=true
      fi
      ;;
    (*)
      # The line is not useful, we skip it
      ;;
  esac
done < <(./../naiveSrc/colorScript.sh "$numvertices" -c "$coloring" 2>/dev/null) 3< <(./colorScript.sh "$numvertices" -c "$coloring" 2>/dev/null)

if [[ "$diff" == true ]]; then
  echo "Outputs differ."
else
  echo "Outputs match, optimized algorithm is correct."
fi



