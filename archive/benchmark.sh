#!/bin/bash

# Change the working directory to this one.
# This makes sure one can run this script from a different directory.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1

n="$1"
path="$2"
shift 2

echo "Starting benchmark for $path with $@ as arguments."
echo ""

LOOPS=3

printf "%-15s | %-15s\n" "Configuration" "Time (ms)"
echo "--------------------------------------------------"

configs=("proper" "odd" "iUMo" "iUMc" "pUMo" "pUMc" "iCFo" "iCFc" "pCFo")

start_time=$(date +%s%N)



for config in "${configs[@]}"; do
  TOTAL_TIME=0
  for ((i=1; i<=LOOPS; i++)); do
    START=$(date +%s%N)
    "./$path" "$n" -c "$config" -o "$@" >/dev/null 2>&1
    END=$(date +%s%N)
    TOTAL_TIME=$((TOTAL_TIME + (END - START)))
  done
  AVG_TIME=$(( TOTAL_TIME / (LOOPS * 1000000) ))
  printf "%-15s | %-15s\n" "$config" "$AVG_TIME"
done



echo "--------------------------------------------------"

end_time=$(date +%s%N)

duration=$(((end_time - start_time) / 1000000))
printf "Total execution time: $duration ms\n\n"
