#!/bin/bash

# The working directory is supposed to be from the main code directory

source ./scripts/utils.sh

# The first (and only argument) should be the extension type of the images
if [[ "$1" != "" ]]; then
  mkdir -p images
  # We also remove all previous files from the directory (-f ignores no file error)
  rm -f images/*

  activate_venv

  # Run Python script with stdin
  "venv/bin/python" scripts/graph6ToImage.py "images/" "$show" </dev/stdin
else
  cat
fi
