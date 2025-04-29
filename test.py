import subprocess
import os

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
    exe_path = os.path.join(current_dir, 'MCDPP.exe')

    # Execute solver
    result = subprocess.run([exe_path,
                            str(base_graph_path),
                            str(demand_graph_path),
                            str(n_sub),
                            str(n_iter_half),
                            str(n_sub_variant),
                            str(lambda_option),
                            str(VZK_option)])

    return True

# Current directory path
current_dir = os.path.dirname(os.path.abspath(__file__))

# Path to the .exe file for solving Minimum Cost Disjoint Path Problem (MCDPP)
#exe_path = os.path.join(current_dir, 'MCDPP.exe')

# Paths to base graph and demand graph files as arguments for solver
base_graph = 'klein.gra'
demand_graph = 'bedarf4.gra'
base_graph_path = os.path.join(current_dir, 'Testdateien', base_graph)
demand_graph_path = os.path.join(current_dir, 'Testdateien', demand_graph)

# Solver attributes
n_sub = 200
n_iter_half = 10
n_sub_variant = 40
lambda_option = 2
VZK_option = 1

# Execute solver
# result = subprocess.run([exe_path,
#                          base_graph_path,
#                          demand_graph_path,
#                          str(n_sub),
#                          str(n_iter_half),
#                          str(n_sub_variant),
#                          str(lambda_option),
#                          str(VZK_option)])

execute_MCDPP_solver(base_graph_path,
                     demand_graph_path,
                     n_sub,
                     n_iter_half,
                     n_sub_variant,
                     lambda_option,
                     VZK_option)
