#!/bin/bash

# Change the working directory to this one.
# This makes sure one can run this script from a different directory.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1

echo "Starting benchmark."
n="$1"
path="$2"
shift 2

LOOPS=3

printf "%-15s | %-15s\n" "Configuration" "Time (ms)"
echo "--------------------------------------------------"

configs=("proper" "odd" "iUMo" "iUMc" "pUMo" "pUMc" "iCFo" "iCFc" "pCFo")

TIMEFORMAT='%R'

start_time=$(date +%s%N)



for config in "${configs[@]}"; do
    TOTAL_TIME=0
    for ((i=1; i<=LOOPS; i++)); do
        START=$(date +%s%N)
        "./$path" "$n" -c "$config" -o "$@" >/dev/null 2>&1
        END=$(date +%s%N)
        
        DIFF=$(( (END - START) / 1000000 ))
        TOTAL_TIME=$((TOTAL_TIME_NAUTY + DIFF))
    done
    AVG_TIME=$(echo "scale=3; $TOTAL_TIME / $LOOPS" | bc)
    printf "%-15s | %-15s\n" "$config" "$AVG_TIME"
done



echo "--------------------------------------------------"

end_time=$(date +%s%N)

duration=$(((end_time - start_time) / (1000000 * LOOPS)))
echo "Total execution time: $duration ms"
