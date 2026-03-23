#!/bin/bash

# Change the working directory to this one.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1

### The generation of the cycles in graph6 format
gen_cycle_graphs() {
	# Check if venv has been created
	if [ ! -d "venv" ]; then
		echo "Error: venv hasn't been created yet."
		exit 1
	fi
	source venv/bin/activate

	python3 - <<'PY'
import networkx as nx

with open("cycles.g6", "w", encoding="utf-8") as f:
    for n in range(3, 21):
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
gen_cycle_graphs | ./color.sh -c "$coloring" -m pipe --raw | while read -r line; do
	echo "  C_$index: $line"
	index=$((index + 1))
done
