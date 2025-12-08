#!/bin/bash

# Change the working directory to this one.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1


gen_subdivision() {
  # Check if venv has been created
  if [ ! -d "venv" ]; then
    echo "Error: venv hasn't been created yet."
    exit 1
  fi
  source venv/bin/activate
  "venv/bin/python" subdivideGraph.py
}

generate_graphs() {
  while IFS=": " read -r graph cnumber; do
    if [[ "$cnumber" == 4 ]]; then
      echo "$graph"
    fi
  done < <(./colorScript.sh "$1" -c proper -p 2>/dev/null)
}

generate_graphs "$1" | gen_subdivision | ./colorScript.sh -m "pipe" -c iCFo






