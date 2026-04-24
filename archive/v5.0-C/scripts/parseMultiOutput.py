import re
import sys
import os

def parse_file(relative_path):
    results = {}
    time = 0.0
    full_count = 0

    pattern = re.compile(r'^\s*(\d+).*?(\w+)=([\d.]+)')
    
    try:
        with open(relative_path, 'r') as file:
            for line in file:
                match = pattern.search(line)
                if match:
                    count_val = int(match.group(1))
                    key = match.group(2)
                    value_val = match.group(3) 

                    if key == "chrom":
                        results[value_val] = results.get(value_val, 0) + count_val
                    elif key == "cpu":
                        time = max(time, float(value_val))
                        full_count += count_val
    except FileNotFoundError:
        print(f"Error: The file at {relative_path} was not found.")
        sys.exit(1)
    
    return results, full_count, time

def build_output(d, count, time):
    s = ""
    for key, value in d.items():
        s += f"  {value} graphs : chrom={key}\n"
    s += f"{count} graphs altogether; cpu={time:.2f} sec\n"
    # print(d)
    return s

def update_file(relative_path, new_content):
    with open(relative_path, 'w') as file:
        file.write(new_content)

if __name__ == "__main__":        
    path = sys.argv[1]

    results, count, time = parse_file(path)
    new_content = build_output(results, count, time)
    update_file(path, new_content)