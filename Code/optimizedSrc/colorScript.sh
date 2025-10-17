#!/bin/bash

# We make sure to recompile the code
javac Main.java
echo "Code compiled."

# Default values for our flags
coloring=""
filter=""
manual=""


# Parse options
while getopts ":c:f:m:n:" opt; do
  case $opt in
    c)
      # shellcheck disable=SC1068
      coloring="$OPTARG"
      ;;
    f)
      filter="$OPTARG"
      ;;
    m)
      manual="$OPTARG"
      ;;
    n)
      n=$OPTARG
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo "This flag (-$OPTARG) requires an argument." >&2
      exit 1
      ;;
  esac
done

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
  ./../../plantri55/plantri -g $n | java Main $coloring
else
  echo "$manual" | java Main $coloring
fi


