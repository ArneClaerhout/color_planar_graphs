#!/bin/bash

# Default values for our flags
coloring=""
filter=""
manual=""
raw=false

# Parse options
POSITIONAL_ARGS=()

# Loop over all flags and their values
while [[ $# -gt 0 ]]; do
  case $1 in
    -n) # Number of vertices, is only used if there is no filter given
      n="$2"
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
  if [[ -z "$n" ]]; then
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
  ./../../plantri55/plantri -g $n | java Main $coloring $raw
else
  echo "$manual" | java Main $coloring $raw
fi


