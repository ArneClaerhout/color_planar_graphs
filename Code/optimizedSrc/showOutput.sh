#!/bin/bash

# Change the working directory to this one.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1

# Create virtual environment if missing
if [ ! -d "venv" ]; then
    python3 -m venv venv
fi

source venv/bin/activate

pip install -q --upgrade pip
pip install -q networkx
pip install -q matplotlib

# Make sure the images folder is created.
mkdir -p images

# Run Python script with stdin
python3 graph6_to_svg.py


