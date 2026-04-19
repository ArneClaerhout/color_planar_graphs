#!/usr/bin/env python3
import sys
import os
import networkx as nx
import matplotlib.pyplot as plt
import urllib.parse
import tikzplotlib


def convert_color(color_number):
    color_to_color = ["blue", "orange", "green", "red", "purple", "brown", "pink", "gray", "olive", "cyan", "black"]
    return color_to_color[int(color_number) - 1]

if __name__ == "__main__":

    count = 0

    for line in sys.stdin:
        # Create a graph image for each line in the stdin

        line = line.strip()
        if not line:
            continue

        last_bracket_idx = line.rfind(']')
        main_part = line[:last_bracket_idx + 1]
        if last_bracket_idx + 1 < len(line):
            extra_info = line[last_bracket_idx + 1:].strip()
        else:
            extra_info = ""

        # We get the data from the input
        parts = main_part.split(" ")
        try:
            colors_numbers = list(map(str.strip, "".join(parts[1:]).strip('][').replace('"', '').split(',')))
            colors = list(map(convert_color, colors_numbers)) # Colour 0 = index -1 -> last colour
        except:
            colors = []

        graph6_string = parts[0]

        path = sys.argv[1]
        input_format = sys.argv[2]
        output_path = f"{path}graph_{count}.{input_format}"
        # print(f"Generating {output_path} ...")

        # Create the graph
        try:
            G = nx.from_graph6_bytes(graph6_string.encode())
        except Exception as e:
            print(f"Error reading graph6 string '{graph6_string}': {e}")
            continue

        # We give each vertex a colour attribute to also show
        for i, color in enumerate(colors_numbers):
            G.nodes[i]['color'] = colors_numbers[i]

        # Plot the graph
        fig, ax = plt.subplots(figsize=(6,6))
        pos = nx.planar_layout(G)

        # Give each vertex the respective colour
        nx.draw(G, pos, ax=ax, with_labels=False, node_color=colors, edge_color='gray', node_size=500)
        # Give the labels as well
        nx.draw_networkx_labels(G, pos, labels=nx.get_node_attributes(G, 'color'), font_color='white', ax=ax)

        ax.set_title(graph6_string, fontsize=14, pad=20)
        if extra_info != "":
            fig.text(0.5, 0.02, extra_info, ha='center', fontsize=10)
            plt.tight_layout(rect=[0, 0.05, 1, 1])

        if input_format == "tex":
            # Save as TikZ
            tikzplotlib.save(output_path, figure=fig, extra_axis_parameters={
                "mark size=5pt",
            })

        else:
            # We take the format as the second argument (the first is always the name of the program)
            plt.savefig(output_path, format=input_format)

        plt.close()
        count = count + 1
