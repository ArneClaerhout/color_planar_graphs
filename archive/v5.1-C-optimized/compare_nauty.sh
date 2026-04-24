#!/bin/bash


# Change the working directory to this one.
# This makes sure one can run this script from a different directory.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir" || exit 1


pattern="nauty"
nauty_path=$(find ../../.. -maxdepth 2 -type d -name "*${pattern}*" | head -n 1)
if [[ -z "$nauty_path" ]]; then
	echo >&2 "Nauty not found. This will not be able to use it."
fi

pattern="plantri"
plantri_path=$(find ../../.. -maxdepth 2 -type d -name "*${pattern}*" | head -n 1)
if [[ -z "$plantri_path" ]]; then
	echo >&2 "Error: Plantri not found."
	exit 1
fi

gen_range_graphs() {
	if [[ "$raw" == false ]]; then
		# Only if we are not in raw mode do we print this.
		echo "Generating graphs from $1 to $2 vertices." >&2 # We print to stderr, so this isn't on stdout
	fi
	for num in $(seq "$1" "$2"); do
		"./$plantri_path/plantri" -g "$num" 2>/dev/null # We get rid of the extra printing to the terminal
	done
}

# Configuration
VERTICES_LIST=(6 7 8 9 10 11 12 13 14 15 16) # Number of vertices to test
RUNS=5                        # Number of times to run each test for averaging

# Header for output
printf "%-10s | %-15s | %-10s | %-10s\n" "Vertices" "Your Program (s)" "Nauty (s)" "Ratio"
echo "------------------------------------------------------------"

for N in "${VERTICES_LIST[@]}"; do
    # 1. Benchmark Your Program
    TOTAL_TIME_MY=0
    for ((i=1; i<=RUNS; i++)); do
        # Use 'date' for millisecond precision
        START=$(date +%s%N)
        ./color.sh "$N" -c proper -o > /dev/null 2>&1
        END=$(date +%s%N)
        
        DIFF=$(( (END - START) / 1000000 )) # Convert to milliseconds
        TOTAL_TIME_MY=$((TOTAL_TIME_MY + DIFF))
    done
    AVG_MY=$(echo "scale=3; $TOTAL_TIME_MY / ($RUNS * 1000)" | bc)

    # 2. Benchmark Nauty (dreadnaut)
    # We generate a graph and find its automorphism group (the 'x' command)
    TOTAL_TIME_NAUTY=0
    for ((i=1; i<=RUNS; i++)); do
        # Create a simple dreadnaut command: n=N, generate random graph, execute
        # 'n=N' sets vertices, 'R' makes random, 'x' runs nauty, 'q' quits
        START=$(date +%s%N)
        gen_range_graphs "$N" "$N" | "./$nauty_path/countg" --N > /dev/null 2>&1
        END=$(date +%s%N)
        
        DIFF=$(( (END - START) / 1000000 ))
        TOTAL_TIME_NAUTY=$((TOTAL_TIME_NAUTY + DIFF))
    done
    AVG_NAUTY=$(echo "scale=3; $TOTAL_TIME_NAUTY / ($RUNS * 1000)" | bc)

    # 3. Calculate Ratio (avoiding division by zero)
    if [[ "$AVG_NAUTY" != "N/A" ]] && (( $(echo "$AVG_MY > 0" | bc -l) )); then
        RATIO=$(echo "scale=2; $AVG_NAUTY / $AVG_MY" | bc)
    else
        RATIO="N/A"
    fi

    # Print results for this vertex count
    printf "%-10d | %-16s | %-10s | %-10s\n" "$N" "$AVG_MY" "$AVG_NAUTY" "$RATIO"
done

