#!/bin/bash

# Change the working directory to this one.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1

# Create virtual environment if missing
if [ ! -d "venv" ]; then
    python3 -m venv venv
fi

# Enter virtual environment
source venv/bin/activate

# Install needed libraries silently (-q)
pip install -q --upgrade pip
pip install -q networkx
pip install -q matplotlib

# Make sure the images folder is created.
mkdir -p images

# We also remove all previous files from the directory (-f ignores no file error)
rm -f images/*

# Run Python script with stdin
"venv/bin/python" graph6_to_image.py "$1"


