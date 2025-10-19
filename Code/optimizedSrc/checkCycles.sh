#!/bin/bash

gen_cycle_graphs() {
python3 -m venv venv && source venv/bin/activate
pip install -q networkx

python3 - <<'PY'
import networkx as nx

with open("cycles.g6", "w", encoding="utf-8") as f:
    for n in range(3, 16):
        s = nx.to_graph6_bytes(nx.cycle_graph(n)).decode().strip()
        if s.startswith(">>graph6<<"):
            s = s[len(">>graph6<<"):]
        print(s)
        f.write(s + "\n")
PY
# We generate cycle graphs in python
}

coloring="$1"

echo "Finding $1 chromatic numbers for cycle graphs:"
index=3
gen_cycle_graphs | ./colorScript.sh -c "$coloring" -m stream --raw |  while read -r line ; do
                                                                          echo "  C_$index: $line"
                                                                          index=$((index+1))
                                                                      done