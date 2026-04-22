#!/bin/bash

# Change the working directory to this one.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1


#######################
###### FUNCTIONS ######
#######################

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

check_nauty() {
  pattern="nauty"
  dir=$(find ../.. -maxdepth 1 -type d -name "*${pattern}*" | head -n 1)
  if [[ -z "$dir" ]]; then
    >&2 echo "Error: Nauty not found."
    exit 1
  fi
  echo "$dir"
}


use_nauty() {

  # First argument for this script is the amount of vertices
  nauty_path=$(check_nauty)
  #echo "$nauty_path"

  output2=$(gen_range_graphs "$startn" "$endn" | "./$nauty_path/countg" --N 2>/dev/null | sed '$d' | tr -d '[:space:]')
  ## We also get rid of the extra printing to the terminal
  echo -n "  Nauty done"
  output1=$(./colorScript.sh "$startn:$endn" -c proper --raw --overview | sed '$d' | tr -d '[:space:]')
  printf ", own program done.\n\n"

  # The two outputs from nauty and my own program, stripped of spaces and the last line
  # We now compare the two (in terms of correctness)
  if [[ "$output1" == "$output2" ]]; then
    echo "  Outputs match, optimized algorithm is correct."
  else
    echo "  Outputs differ."
  fi

}


# We get the number of vertices
if [[ "$1" == *:* ]]; then
  # Split the argument into start and end
  IFS=':' read -r startn endn <<< "$1"
else
  # Only one value given; use it as both start and end
  startn="$1"
  endn="$1"
fi
shift 1


########################
###### COMPARISON ######
########################

declare -a colorings=()

# "pCFo" "pCFc" "pUMo" "pUMc" "odd" "proper"
# Proper has been checked from 3:17, it is correct
# Others have been checked from 3:12
# iCFo, iCFc, iUMo, iUMc to 14

while [[ $# -gt 0 ]]; do
  if [[ "$1" == "all" ]]; then
      colorings+=("proper" "odd" "pUMo" "pUMc" "pCFo" "pCFc" "iUMo" "iUMc" "iCFo" "iCFc")
  else
      colorings+=("$1")
  fi
  shift 1
  # We add all arguments as colorings
done

## loop through above array
for i in "${colorings[@]}"
do
  echo "$i:"
  if [[ "$i" == "proper" ]]
  # With proper colorings, we can use nauty
  then
    use_nauty
  else
    ./checkNaiveOutputs.sh "$startn:$endn" -c "$i" | sed 's/^/  /'
  fi
done






