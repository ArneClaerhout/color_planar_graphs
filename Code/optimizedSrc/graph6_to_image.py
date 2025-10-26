#!/usr/bin/env python3
import sys
import os
import networkx as nx
import matplotlib.pyplot as plt
import urllib.parse


def convert_color(color_number):
    color_to_color = ["blue", "orange", "green", "red", "purple", "brown", "pink", "gray", "olive", "cyan"]
    return color_to_color[int(color_number) - 1]


count = 0

for line in sys.stdin:
    # Create a graph image for each line in the stdin

    line = line.strip()
    if not line:
        continue

    # We get the data from the input
    line = line.split(" ")
    colors_numbers = list(map(str.strip, "".join(line[1:]).strip('][').replace('"', '').split(',')))
    colors = list(map(convert_color, colors_numbers))

    graph6_string = line[0]

    input_format = sys.argv[1]
    output_path = f"images/graph_{count}.{input_format}"
    # print(f"Generating {output_path} ...")

    # Create the graph
    try:
        G = nx.from_graph6_bytes(graph6_string.encode())
    except Exception as e:
        print(f"Error reading graph6 string '{graph6_string}': {e}", file=sys.stderr)
        continue

    # We give each vertex a color attribute to also show
    for i, color in enumerate(colors_numbers):
        G.nodes[i]['color'] = colors_numbers[i]

    # Plot the graph
    plt.figure(figsize=(4, 4))
    pos = nx.planar_layout(G)
    # Draw the graph, with the respective color
    nx.draw(G, pos, with_labels=False, node_color=colors, edge_color='gray', node_size=700)
    # Give each vertex a label equal to the color
    nx.draw_networkx_labels(G, pos, labels=nx.get_node_attributes(G, 'color'), font_color='white')

    # We take the format as the second argument (the first is always the name of the program)
    plt.savefig(output_path, format=input_format)
    plt.close()
    count = count + 1
