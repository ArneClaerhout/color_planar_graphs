#!/bin/bash

# Change the working directory to this one.
# This makes sure one can run this script from a different directory.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1

##################
###### HELP ######
##################

show_help() {
cat << EOF
  Usage:
    ${0##*/} NUMBER_OF_VERTICES [OPTIONS]

  Description:
    This script helps the user in running the coloring algorithm provided in Main.java.
    The user of this script can choose to use multiple provided options.

    Full explanation of the entire repository can be found here:
        https://github.com/ArneClaerhout/Bachelorproef

  Positional Arguments:
    NUMBER_OF_VERTICES
        The amount of vertices the to be colored graphs should have.
        This can either be a range, or just a single number.
        The range should be given in the following format:

        MIN_VERTICES:MAX_VERTICES

  Options:
    -h, --help
        Show this help message and exit.

    -c, --coloring
        The chosen coloring to use.
        The default coloring is the proper coloring.
        Examples: proper, odd, iUMo, pCFc, ...

    -m, --manual GRAPH
        Whether the user wants to manually input graphs.
        The value for this option should be the graph to color in graph6 format.
        If the user would like to input the graphs through a pipe,
        the GRAPH value should be "pipe".

    -f, --filter MIN_COLORS
        Adds a filter to the outputted graphs.
        MIN_COLORS is the minimum amount of colors needed before the graph gets to be output to the terminal.
        When the chromatic number of a graph is strictly smaller than this value, it gets skipped.

    -o, --overview
        Gives an overview of all the outputted graphs instead of
        actually printing all graphs and their corresponding chromatic numbers.
        This also keeps track of the needed time for the algorithm as a whole.

    -r, --raw VALUE
        Alternates the output of the algorithm to a more raw output.
        VALUE should be a value equal to 1, 2 or 3.
          - A value of 1 makes sure the output is only the chromatic numbers of the graphs.
          - A value of 2 only outputs the graph6 strings of the graphs (only useful when filtering).
          - A value of 3 only outputs the graph6 strings followed by the used colors ordered by index.

    -s, --show FORMAT
        Outputs an image of the outputted graphs to the directory images/ .
        The file extension of these images should be given by FORMAT.
        The default value for FORMAT is svg.

  Examples:
    Run the script with a file of graph6 strings:
        cat input.txt | ./${0##*/} --manual pipe

    Run the script with common options:
        ./${0##*/} 3:10 -c pUMo -f 6 --show

    Other examples can be found on the Github page.

  Notes:
    - Arguments can be combined with both short and long forms. The order of the arguments doesn't matter either.

  Author:
    Arne Claerhout
EOF
}

#########################
###### DIRECTORIES ######
#########################

pattern="nauty"
nauty_path=$(find ../.. -maxdepth 1 -type d -name "*${pattern}*" | head -n 1)
if [[ -z "$nauty_path" ]]; then
  >&2 echo "Error: Nauty not found."
  exit 1
fi


pattern="plantri"
plantri_path=$(find ../.. -maxdepth 1 -type d -name "*${pattern}*" | head -n 1)
if [[ -z "$plantri_path" ]]; then
  >&2 echo "Error: Plantri not found."
  exit 1
fi


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
    "./$plantri_path/plantri" -g "$num" 2>/dev/null # We get rid of the extra printing to the terminal
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
  if [[ "$progressview" == true ]]; then
    pv | java Main "$coloring" "$overview" "$raw" "$minChrom" | show_func
  else
    java Main "$coloring" "$overview" "$raw" "$minChrom" | show_func
  fi

}


choose_incoming_graphs() {
  # First we check whether we have a filter file

  if ! [[ "$filter" =~ ^-?[0-9]+$ ]]; then
    # The filter is not an integer
    # We parse the given filter
    # Check if venv has been created
      if [ ! -d "venv" ]; then
          echo "Error: venv hasn't been created yet."
          exit 1
      fi
      source venv/bin/activate

      change=""
      changeoverview=false
      "venv/bin/python" parseFilter.py "$filter" | while read -r line; do
          # We first parse the line
          # This changes all the variables to the ones given in the filter
          read -r raw overview n coloring minChrom rest <<< "$line"
          if [[ "$change" != "" ]]; then

            # Last cycle we got a filter name
            echo ":raw $raw"
            echo ":overview $overview"
            echo ":coloring $coloring"
            echo ":minChrom $minChrom"
            if [[ "$changeoverview" == true ]]; then
              echo ":update overview"
              changeoverview=false
            fi
            if [[ "$raw" == 0 ]]; then
              echo ":print $change"
            fi
            change=""
          fi
          if [[ "$overview" != True && "$overview" != False ]]; then
            # overview isn't a boolean, we update the arguments
            change="$overview"
            if [[ "$raw" != 0 ]]; then
              changeoverview=true
            fi
          fi

          "./$plantri_path/plantri" -g "$n" 2>/dev/null | eval "\"./$nauty_path/pickg\" $rest" 2>/dev/null
      done

      # We deactivate the venv
      deactivate
  elif [[ -z "$manual" ]]; then
    # Not manual
    gen_range_graphs "$startn" "$endn" "$raw"
  elif [[ "$manual" == "pipe" ]]; then
    # Own stream chosen
    read_stdin
  else
    echo "$manual"
  fi


}


################################
###### NUMBER OF VERTICES ######
################################

# Default values for our flags
coloring=""
filter=0
minChrom=0
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
    -h|--help)
      show_help
      exit 0
      ;;
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
    -pv|progressview)
      progressview=true
      shift 1
      ;;
    -o|--overview)
      overview=true
      shift
      ;;
    -r|--raw)
      if [[ -z "${2:-}" || "$2" == -* ]]; then
          # No value given, we give it 1
          raw=1
          shift 1
      else
        raw="$2"
        shift 2
      fi
      ;;
    -s|--show)
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
# javac Graphs.Main.java

### RAW-CHECK
#if [[ "$raw" == 0 ]]; then
#  # Only if we are not in raw mode do we print this.
#  echo "Code compiled."
#elif [[ "$raw" -ne 1 && "$raw" -ne 2 && "$raw" -ne 3 ]]; then
#  # We only want three raw options
#  raw=1
#fi

### SHOW-CHECK
if [[ "$show" == true ]]; then
  # If show is chosen, we always pick these values
  raw=3
  overview=false
  progressview=false
fi

### FILTER CHECK
if [[ "$filter" =~ ^-?[0-9]+$ ]]; then
  # Filter is a number
  minChrom="$filter"
fi

### NUMBER OF VERTICES CHECK
if [[ -z "$startn" && -z "$manual" && "$filter" =~ ^-?[0-9]+$ ]]; then
  # There is no n given and we don't want to manually give a graph
  # And we also haven't recieved a filter file (as it is just a number) -> ERROR
  # We only check startn, as both are either filled or not filled
  echo "Error: Number of vertices wasn't given."
  exit 1
fi

### EXECUTION

choose_incoming_graphs | java_alg


# Extra output for show functionality to signal the end of the generation.
if [[ "$show" == true ]]; then
  ./showOutput.sh
  echo "Generated all images."
fi




