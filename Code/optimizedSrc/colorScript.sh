#!/bin/bash

# Change the working directory to this one.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1


#######################
###### FUNCTIONS ######
#######################

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
    ./showOutput.sh "$show_value"
  else
    cat
  fi
}

java_alg() {
  java Main "$coloring" "$overview" "$raw" "$filter" | show_func
}


################################
###### NUMBER OF VERTICES ######
################################

# Default values for our flags
coloring=""
filter=0
manual=""
raw=0
overview=false
progressview=false
show=false
show_value="svg"

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


#######################
###### ARGUMENTS ######
#######################

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
          # No value given, we give it 1
          raw=1
          shift 1
      else
        raw="$2"
        shift 2
      fi
      ;;
    --show)
      show=true
      if [[ -z "${2:-}" || "$2" == -* ]]; then
        # We didn't get a value
        show_value="svg"
        shift 1
      else
        show_value="$2"
        shift 2
      fi
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


#######################
###### ALGORTIHM ######
#######################


# We make sure to recompile the code
javac Main.java

### RAW-CHECK
if [[ "$raw" == 0 ]]; then
  # Only if we are not in raw mode do we print this.
  echo "Code compiled."
elif [[ "$raw" -ne 1 && "$raw" -ne 2 ]]; then
  # We only want two raw options
  raw=1
fi

### SHOW-CHECK
if [[ "$show" == true ]]; then
  # If show is chosen, we always pick these values
  raw=2
  overview=false
  progressview=false
  showcommand=./showOutput.sh
fi

### NUMBER OF VERTICES CHECK
if [[ -z "$startn" && -z "$manual" ]]; then
  # There is no n given and we don't want to manually give a graph -> ERROR
  # We only check startn, as both are either filled or not filled
  echo "Error: Number of vertices wasn't given."
  exit 1
fi


### PROGRESS BAR
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


### NO PROGRESS BAR
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


# Extra output for show functionality to signal the end of the generation.
if [[ "$show" == true ]]; then
  echo "Generated all images."
fi




