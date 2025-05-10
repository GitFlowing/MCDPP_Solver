import subprocess
import os
import matplotlib.pyplot as plt
import networkx as nx

def execute_MCDPP_solver(base_graph, demand_graph, n_sub, n_iter_half, n_sub_variant, lambda_option, VZK_option):
    """
    This function executes the MCDPP solver with specified parameters:
    - base_graph: Path to the base graph of MCDPP
    - demand_graph: Path to the demand graph of MCDPP
    - n_sub: Number of iterations for subgradient calculation (lower bound)
    - n_iter_half: Number of iterations for the step size update of subgradient calculation (lower bound)
    - n_sub_variant: Number of iterations for the subgradient calculation for determination of branch node
      for Branch and Bound procedure (only for VZK_option = 3)
    - lambda_option: Option for the starting lambda value for subgradient calculation
      1 = set lambda to 0
      2 = set lambda to the value, for which parent node of Branch and Bound has maximum lower bound in subgradient calculation
      3 = set lambda to the value, for which parent node of Branch and Bound has minimum number of conflicts for embedded paths
    - VZK_option: Option for choosing the next branch node for the Branch and Bound procedure
      1 = choose node in base_graph with the maximum number of conflicts for embedded paths at lambda_option
      2 = choose middle node in base_graph of the embedded demand with the shortest way at lambda_option
      3 = take all conflict nodes of the embedded demands at lambda_option,
          calculate their lower bounds with subgradient calculation,
          choose the node with the maximum lower bound for the next branch node

    It constructs the command to run the solver and captures the solution of
    the solver in ergebnis_output.txt file.
    """

    # Current directory path
    current_dir = os.path.dirname(os.path.abspath(__file__))

    # Path to the .exe file for solving Minimum Cost Disjoint Path Problem (MCDPP)
    exe_path = os.path.join(current_dir, './mcdpp')

    # Execute solver
    result = subprocess.run([exe_path,
                            str(base_graph),
                            str(demand_graph),
                            str(n_sub),
                            str(n_iter_half),
                            str(n_sub_variant),
                            str(lambda_option),
                            str(VZK_option)])

    return True


def extract_nodes_edges(graph):
    """
    extract nodes, edges and costs of a graph in a .gra file and return it as dictionary
    """
    nodes = {}
    edges = {}
    edge_cost = {}


    with open(graph, 'r') as file:
        # read line by line and ignore hashtags as comments and empty lines
        counter = 0
        node_count = 0
        edge_count = 0

        for line in file:
            # ignore hashtags, empty lines or third line
            if line.startswith('#') or line.strip() == '':
                continue
            # node count is first line
            elif counter == 0:
                node_count = line.strip().split()[0]
                counter += 1
            # edge count is second line
            elif counter == 1:
                edge_count = line.strip().split()[0]
                counter += 1
            # graph direction is third line and can be ignored
            elif counter == 2:
                counter += 1
                continue
            # after third line follows node data
            elif counter > 2 and counter <= int(node_count) + 2:
                # read node data
                parts = line.strip().split()
                node_id = parts[0]
                x = float(parts[1])
                y = float(parts[2])
                # append to nodes dictionary
                nodes.update({node_id: [x, y]})
                counter += 1
            # after node data follows edge data and nothing more
            else:
                # read edge data
                parts = line.strip().split()
                edge_id = parts[0]
                node1 = parts[1]
                node2 = parts[2]

                # Demand graph has no cost, but base graph has cost
                cost = 0
                if len(parts) >= 4:
                    cost = float(parts[3])
                # append to edges dictionary
                edges.update({edge_id: [node1, node2]})
                edge_cost.update({edge_id: cost})
                counter += 1

    return nodes, edges, edge_cost


def extract_solution(result_file):
    """
    This function extracts the solution from the result file of the MCDPP solver.
    It reads the file and extracts time of the solver, total cost and the paths.
    Paths are here the indices of the edges in the base graph.
    """

    result = {}
    paths = []
    counter = 0
    with open(result_file, 'r') as file:
        for line in file:
            if counter == 0:
                # read time of the solver
                result["time"] = float(line.strip().split()[-1])
                counter += 1
            elif counter == 1:
                # read total cost
                result["total_cost"] = float(line.strip().split()[-1])
                counter += 1
            elif counter == 2:
                # jump over text
                counter += 1
                continue
            else:
                # read paths
                parts = line.strip().split()
                path = [int(x) for x in parts]
                paths.append(path)

    # append paths to result
    result['paths'] = paths

    return result

def plot_graph(graph,demand_graph, sol_path="", fig_size_x=20, fig_size_y=25, fig_dpi=75,
               node_size=500, edge_size=3, terminal_size=500, edge_labels = False,
               edge_lables_size=10, edge_lable_pos=0.5):
    """
    It creates a plot of the base graph or demand graph or base graph with embedded solution.
    The parameters are:
    - graph: data path of graph (base graph or demand graph)
    - demand_graph: data path of demand graph
    - sol_path: data path of solution
    - fig_size_x: width of graph plot picture in inch
    - fig_size_y: height of graph plot picture in inch
    - fig_dpi: picture dots per inch
    - node_size: size of nodes in graph in pt^2
    - edge_size: size of edges in graph in pt
    - terminal_size: size of demand nodes in pt^2
    - edge_labels: if True, the edges are labeled with their cost
    - edge_lables_size: size of edge labels in pt
    - edge_lable_pos: position of edge labels in graph
    """
    # Extract nodes and edges from the graph
    nodes, edges, edge_cost = extract_nodes_edges(graph)
    demand_nodes, demand_edges, demand_edge_cost = extract_nodes_edges(demand_graph)

    node_ids = list(nodes.keys())
    edges_list = list(edges.values())
    edge_cost_list = list(edge_cost.values())
    demand_node_ids = list(demand_nodes.keys())

    # Create graph
    G = nx.Graph()
    G.graph["Name"] = "Graph"
    G.add_nodes_from(node_ids)
    G.add_edges_from(edges_list)

    # Create figure and axis objects
    fig, ax = plt.subplots(figsize=(fig_size_x, fig_size_y), dpi=fig_dpi)
    ax.axis('off')
    colors = ["red", "green", "blue", "darkorange", "goldenrod", "purple", "cyan", "magenta", "brown", "olive",
          "yellow", "chocolate", "darkkhaki", "lawngreen", "turquoise", "darkcyan", "dodgerblue", "blueviolet",
          "indigo", "crimson"]

    # Define Demand nodes as red nodes and other nodes as black nodes
    node_width_vec = [node_size] * len(node_ids)
    node_colors_vec = ['black'] * len(node_ids)
    for i in range(len(demand_node_ids)):
        index = node_ids.index(demand_node_ids[i])
        node_width_vec[index] = terminal_size
        node_colors_vec[index] = 'red'


    # Draw the base graph with demand nodes
    nx.draw_networkx(G, pos=nodes, with_labels=False, ax=ax, node_color = node_colors_vec,node_size = node_width_vec)
    nx.draw_networkx_edges(G, pos=nodes, width=edge_size)


    # Draw Demand graph
    if graph == demand_graph:
        # Draw edges with different colors
        for j in range(len(edges_list)):
            col_index = j % len(colors)
            nx.draw_networkx_edges(G, pos=nodes, edgelist=[edges_list[j]], edge_color=colors[col_index], width=edge_size)
    # Draw base graph with embedded solution
    if graph != demand_graph and sol_path != "":
        # Extract edge indices from solution
        solution = extract_solution(sol_path)
        path_edges_indices =  solution['paths']

        # Create edges [node1, node2] out of edge indices
        paths = []
        for sol_edge in path_edges_indices:
            path = []
            for index in sol_edge:
                path.append(edges_list[index])
            paths.append(path)

        # Draw edges with different colors
        for j in range(len(paths)):
            col_index = j % len(colors)
            nx.draw_networkx_edges(G, pos=nodes, edgelist=paths[j], edge_color=colors[col_index], width=2*edge_size)

    # Draw edge labels with costs
    if edge_labels:
        tuple_edge_list = [tuple(edge) for edge in edges_list]
        edge_label_dict = dict(zip(tuple_edge_list, edge_cost_list))
        nx.draw_networkx_edge_labels(G, pos=nodes, edge_labels=edge_label_dict, font_size=edge_lables_size, label_pos=edge_lable_pos)

    return fig


if __name__ == "__main__":
    # Current directory path
    current_dir = os.path.dirname(os.path.abspath(__file__))
    parent_dir = os.path.join(current_dir, os.pardir)


    # Paths to base graph and demand graph files as arguments for solver
    base_graph = 'deutschland1.gra'
    demand_graph = 'bedarf19.gra'
    base_graph_path = os.path.join(parent_dir, 'Testdateien', base_graph)
    demand_graph_path = os.path.join(parent_dir, 'Testdateien', demand_graph)

    # Solver attributes
    n_sub = 200
    n_iter_half = 10
    n_sub_variant = 40
    lambda_option = 2
    VZK_option = 1

    # Run solver
    execute_MCDPP_solver(base_graph_path,
                        demand_graph_path,
                        n_sub,
                        n_iter_half,
                        n_sub_variant,
                        lambda_option,
                        VZK_option)

    result_time = -1
    result_cost = -1

    # Does solution exists
    solution_exists = True
    with open("ergebnis_output.txt", 'r') as file:
        for line in file:
            if line.startswith("Zeit in s:"):
                result_time = line.split(":")[1].strip()
            if line.startswith("MCDPP besitzt keine Loesung."):
                solution_exists = False
                break

    print("========")
    print("Results")
    print("========")
    print(" ")
    print("Time: ", result_time)
    # Create graphs, when solution exists
    if solution_exists:
        result = extract_solution('ergebnis_output.txt')
        result_costs = result['total_cost']
        print("Costs: ", result_costs)
        fig = plot_graph(base_graph_path, demand_graph_path, sol_path="ergebnis_output.txt",
                        edge_labels=False, edge_lables_size=30, edge_lable_pos=0.3)
        fig.savefig("graph.jpg", format="jpg")
    else:
        print("MCDPP has no solution.")
