#!/bin/bash

# Change the working directory to this one.
# This makes sure one can run this script from a different directory.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1

# Create the venv if hasn't been before
if [ ! -d "venv" ]; then
	python3 -m venv ../venv
fi
echo "Created Virtual Environment..."

# We enter the venv to install all libraries
source ../venv/bin/activate

# Install all used libraries
# Install needed libraries silently (-q)
pip install -q --upgrade pip
pip install -q networkx
pip install -q "matplotlib<3.8"
pip install -q tikzplotlib
pip install -q "webcolors<1.12"
pip install -q sympy

# We turn the venv back off
deactivate

echo "Installed all libraries."
