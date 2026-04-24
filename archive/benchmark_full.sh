#!/bin/bash

# Change the working directory to this one.
# This makes sure one can run this script from a different directory.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1

path="$1"
shift 2

echo "Benchmarking $path for multiple vertices."
echo ""

LOOPS=3
vertices=(6 7 8 9 10 11 12 13 14)
configs=("proper" "odd" "iUMo" "iUMc" "pUMo" "pUMc" "iCFo" "iCFc" "pCFo")

printf "%-17s" "Num vertices"
for config in "${configs[@]}"; do
  printf "| %-17s" "$config time (ms)"
done
printf "| %-17s\n" "Total time (ms)"
echo "----------------------------------------------------------------------------------------------------------"

start_time=$(date +%s%N)


for n in "${vertices[@]}"; do
  printf "%-17s" "$n"
  START_TIME_N=$(date +%s%N)
  for config in "${configs[@]}"; do
    TOTAL_TIME=0
    for ((i=1; i<=LOOPS; i++)); do
      START=$(date +%s%N)
      "./$path" "$n" -c "$config" -o "$@" >/dev/null 2>&1
      END=$(date +%s%N)
      TOTAL_TIME=$((TOTAL_TIME + (END - START)))
    done
    AVG_TIME=$(( TOTAL_TIME / (LOOPS * 1000000) ))
    printf "| %-17s" "$AVG_TIME"
  done
  END_TIME_N=$(date +%s%N)
  n_duration=$(((END_TIME_N - START_TIME_N) / 1000000))
  echo "| $n_duration"
done



echo "--------------------------------------------------"

end_time=$(date +%s%N)

duration=$(((end_time - start_time) / 1000000))
printf "Total execution time: $duration ms\n\n"
