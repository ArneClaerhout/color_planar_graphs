#!/bin/bash

# Change the working directory to this one.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1


# Function that generates the plantri output in a range of vertices
gen_range_graphs() {
  if [[ "$raw" == 0 ]]; then
    # Only if we are not in raw mode do we print this.
    echo "Generating graphs from $1 to $2 vertices." >&2 # We print to stderr, so this isn't on stdout
  fi
  for num in $(seq "$1" "$2"); do
    ./../../plantri55/plantri -g "$num" 2>/dev/null # We get rid of the extra printing to the terminal
  done
}

read_stdin() {
  while IFS= read -r line; do
    echo "$line"
  done
}

show_func() {
  if [[ "$show" == true ]]; then
  ./showOutput.sh
  else
    cat
  fi
}

java_alg() {
  java Main "$coloring" "$overview" "$raw" "$filter" | show_func
}

# Default values for our flags
coloring=""
filter=0
manual=""
raw=0
overview=false
progressview=false
show=false

if [[ "$1" != -* ]]; then
  # Number of vertices is given

  # We extract them
  if [[ "$1" == *:* ]]; then
    # Split the argument into start and end
    IFS=':' read -r startn endn <<< "$1"
  else
    # Only one value given; use it as both start and end
    startn="$1"
    endn="$1"
  fi
  shift 1

fi


# Parse options
POSITIONAL_ARGS=()

# Loop over all flags and their values
while [[ $# -gt 0 ]]; do
  case $1 in
    -c|--coloring)
      if [[ -z "${2:-}" || "$2" == -* ]]; then
          echo "Error: option $1 requires a value"
          exit 1
      fi
      coloring="$2"
      shift 2
      ;;
    -m|--manual)
      if [[ -z "${2:-}" || "$2" == -* ]]; then
          echo "Error: option $1 requires a value"
          exit 1
      fi
      manual="$2"
      if [[ -z "${2:-}" || "$2" == -* ]]; then
          echo "Error: option $1 requires a value"
          exit 1
      fi
      shift 2
      ;;
    -f|--filter)
      if [[ -z "${2:-}" || "$2" == -* ]]; then
          echo "Error: option $1 requires a value"
          exit 1
      fi
      filter="$2"
      shift 2
      ;;
    -pv)
      progressview=true
      shift 1
      ;;
    --overview)
      overview=true
      shift
      ;;
    --raw)
      if [[ -z "${2:-}" || "$2" == -* ]]; then
          echo "Error: option $1 requires a value"
          exit 1
      fi
      raw="$2"
      shift 2
      ;;
    --show)
      show=true
      shift
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

# We make sure to recompile the code
javac Main.java
if [[ "$raw" == 0 ]]; then
  # Only if we are not in raw mode do we print this.
  echo "Code compiled."
elif [[ "$raw" != 1 || "$raw" != 2 ]]; then
  # We only want two raw options
  raw=1
fi

if [[ "$show" == true ]]; then
  # If show is chosen, we always pick these values
  raw=2
  overview=false
  progressview=false
  showcommand=./showOutput.sh

  # We also remove all previous files from the directory
  rm images/*
fi



#echo "Coloring: $coloring"
#echo "Filename: $filename"
#echo "Manual: $manual"

if [[ -z "$startn" && -z "$manual" ]]; then
  # There is no n given and we don't want to manually give a graph -> ERROR
  # We only check startn, as both are either filled or not filled
  echo "Error: Number of vertices wasn't given."
  exit 1
fi

if [[ "$progressview" == true ]]; then

  # We add a progress bar

  if [[ -z "$manual" ]]; then
    # Manual not set
    gen_range_graphs "$startn" "$endn" "$raw" | pv | java_alg
  elif [[ "$manual" == "stream" ]]; then
    # Own stream chosen
    read_stdin | pv | java_alg
  else
    echo "$manual" | pv | java_alg
  fi

else

  if [[ -z "$manual" ]]; then
      # Manual not set
      gen_range_graphs "$startn" "$endn" "$raw" | java_alg
    elif [[ "$manual" == "stream" ]]; then
      # Own stream chosen
      read_stdin | java_alg
    else
      echo "$manual" | java_alg
    fi

fi

if [[ "$show" == true ]]; then
  echo "Generated all images."
fi




