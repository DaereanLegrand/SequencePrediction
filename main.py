import networkx as nx
import matplotlib.pyplot as plt

def plot_sequence_graph(file_path):
    with open(file_path, 'r') as f:
        sequence = f.readline().strip()
        sequence_len = len(sequence)
        
        pairs = []
        for line in f:
            i, j = map(int, line.strip().split())
            pairs.append((i, j))
    
    G = nx.Graph()

    for i, char in enumerate(sequence):
        G.add_node(i, label=char)
    
    for i in range(sequence_len - 1):
        G.add_edge(i, i + 1)

    for i, j in pairs:
        G.add_edge(i, j)

    labels = {i: f"{i}\n{char}" for i, char in enumerate(sequence)}
    
    pos = nx.spring_layout(G)
    plt.figure(figsize=(10, 6))
    nx.draw(G, pos, with_labels=True, labels=labels, node_size=500, font_size=10, font_color="white", font_weight="bold", node_color="skyblue")
    plt.title("Sequence Graph with Contiguous and Specified Pairs")
    plt.show()

file_path = './rna_pairs.txt'
plot_sequence_graph(file_path)
