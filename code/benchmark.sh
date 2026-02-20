#!/bin/bash

# Change the working directory to this one.
# This makes sure one can run this script from a different directory.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1

echo "Starting benchmark."
path="$1"
shift 1

TIMEFORMAT='%R'

start_time=$(date +%s%N)

printf "Proper: "
time "./$path" 14 -c proper -o "$@" >/dev/null 2>&1
printf "Odd: "
time "./$path" 14 -c odd -o "$@" >/dev/null 2>&1
printf "iUMo: "
time "./$path" 14 -c iUMo -o "$@" >/dev/null 2>&1
printf "iUMc: "
time "./$path" 14 -c iUMc -o "$@" >/dev/null 2>&1
printf "pUMo: "
time "./$path" 14 -c pUMo -o "$@" >/dev/null 2>&1
printf "pUMc: "
time "./$path" 14 -c pUMc -o "$@" >/dev/null 2>&1
printf "iCFo: "
time "./$path" 14 -c iCFo -o "$@" >/dev/null 2>&1
printf "iCFc: "
time "./$path" 14 -c iCFc -o "$@" >/dev/null 2>&1
printf "pCFo: "
time "./$path" 14 -c pCFo -o "$@"

end_time=$(date +%s%N)

duration=$(((end_time - start_time) / 1000000))
echo "Execution time: $duration ms"
