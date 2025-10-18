#!/bin/bash

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


# Default values for our flags
coloring=""
filter=""
manual=""
raw=false
overview=false

# Parse options
POSITIONAL_ARGS=()

# Loop over all flags and their values
while [[ $# -gt 0 ]]; do
  case $1 in
    -n) # Number of vertices, is only used if there is no filter given
      # Check if this is done as a range
      if [[ "$2" == *:* ]]; then
        # Split the argument into start and end
        IFS=':' read -r startn endn <<< "$2"
      else
        # Only one value given; use it as both start and end
        startn="$2"
        endn="$2"
      fi
      shift 2
      ;;
    -c|--coloring)
      coloring="$2"
      shift 2
      ;;
    -m|--manual)
      manual="$2"
      shift 2
      ;;
    -f|--filter)
      filter="$2"
      shift 2
      ;;
    --overview)
      overview=true
      shift
      ;;
    --raw)
      raw=true
      shift # We only shift once as there is no value associated with raw
      ;;
    -*|--*)
      echo "Unknown option $1"
      exit 1
      ;;
  esac
done

# We make sure to recompile the code
javac Main.java
if [[ "$raw" == false ]]; then
  # Only if we are not in raw mode do we print this.
  echo "Code compiled."
fi


#echo "Coloring: $coloring"
#echo "Filename: $filename"
#echo "Manual: $manual"

if [[ -z "$filter" ]]; then
  # No filter given
  if [[ -z "$startn" ]]; then
    # We only check startn, as both are either filled or not filled
    echo "Error: No filter or number of vertices given."
    exit 1
  fi
else
  # Filter given, we use this over number of vertices
  # PLACEHOLDER
  n=$((filter + 0)) # Set n to the filter
  # PLACEHOLDER
fi

if [[ -z "$manual" ]]; then
  # Manual not set
  gen_range_graphs "$startn" "$endn" "$raw" | java Main "$coloring" "$overview" "$raw"
else
  echo "$manual" | java Main "$coloring" "$overview" "$raw"
fi


