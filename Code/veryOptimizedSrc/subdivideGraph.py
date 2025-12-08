import networkx as nx
import sys

def subdivide_graph(G):
    S = nx.Graph()
    S.add_nodes_from(G.nodes())

    for i, (u, v) in enumerate(G.edges()):
        w = f"s{i}"          # new vertex for the subdivided edge
        S.add_node(w)
        S.add_edge(u, w)
        S.add_edge(w, v)

    # Removes graph6 header: [10:]
    return nx.to_graph6_bytes(S).decode('utf-8')[10:].strip()

if __name__ == "__main__":

    for line in sys.stdin:
        print(subdivide_graph(nx.from_graph6_bytes(line.strip().encode())))