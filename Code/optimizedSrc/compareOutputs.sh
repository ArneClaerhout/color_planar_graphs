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

gen_cycle_graphs() {
python3 -m venv venv && source venv/bin/activate
pip install -q networkx

python3 - <<'PY'
import networkx as nx

with open("cycles.g6", "w", encoding="utf-8") as f:
    for n in range(3, 13):
        s = nx.to_graph6_bytes(nx.cycle_graph(n)).decode().strip()
        if s.startswith(">>graph6<<"):
            s = s[len(">>graph6<<"):]
        print(s)
        f.write(s + "\n")
PY
# We generate cycle graphs in python
}



if [[ "$1" == *:* ]]; then
  # Split the argument into start and end
  IFS=':' read -r startn endn <<< "$1"
else
  # Only one value given; use it as both start and end
  startn="$1"
  endn="$1"
fi



# First argument for this script is the amount of vertices


output2=$(gen_range_graphs "$startn" "$endn" | ./../../nauty2_9_1/countg --N 2>/dev/null | sed '$d' | tr -d '[:space:]')
## We also get rid of the extra printing to the terminal
echo -n "Nauty done"
output1=$(./colorScript.sh "$1" -c proper --raw --overview | sed '$d' | tr -d '[:space:]')
printf ", own program done.\n\n"

#echo "$output1"
#echo "$output2"

# The two outputs from nauty and my own program, stripped of spaces and the last line
# We now compare the two (in terms of correctness)
if [[ "$output1" == "$output2" ]]; then
  echo "Outputs match, own program is correct."
else
  echo "Outputs differ."
fi

printf "\nFinding chromatic number for cycle graphs:\n"
gen_cycle_graphs | ./colorScript.sh -c iCFo -m stream --raw --overview

