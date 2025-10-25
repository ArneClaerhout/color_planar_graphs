#!/usr/bin/env python3
import sys
import os
import networkx as nx
import matplotlib.pyplot as plt
import urllib.parse

count=0

for line in sys.stdin:
    # Create a graph image for each line in the stdin

    line = line.strip()
    if not line:
        continue

    output_path = f"images/graph_{count}.svg"
    # print(f"Generating {output_path} ...")

    # Create the graph
    try:
        G = nx.from_graph6_bytes(line.encode())
    except Exception as e:
        print(f"Error reading graph6 string '{line}': {e}", file=sys.stderr)
        continue

    # Plot the graph
    plt.figure(figsize=(4, 4))
    nx.draw(G, with_labels=True, node_color='lightblue', edge_color='gray', node_size=700)
    plt.savefig(output_path, format="svg")
    plt.close()
    count = count + 1
