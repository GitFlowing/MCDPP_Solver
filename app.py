import streamlit as st
from MCDPPExecution.execute_MCDPP import execute_MCDPP_solver, plot_graph
import os
import matplotlib.pyplot as plt

# TODO: Implement more simple test instances like in bachelor thesis
# TODO: Implement edge costs display in graph
# TODO: Implement no solution test instance (minimum 3)
# TODO: Implement big test instance (minimum 3) -> overlapping points in visualization + no costs
# TODO: Implement side bar with: MCDPP, MCDPP Solver, Create own MCDPP instance
# TODO: Styling of the website
# TODO: Use in introduction pictures instead of solver.

st.title("Minimum Cost Disjoint Path Problem (MCDPP) Solver")

st.write("""
         The MCDPP describes the following problem: For a given undirected graph with
         non-negative edge costs and a certain number of node pairs,
         paths are to be found on the graph between the node pairs. These paths
         must not intersect. The goal is to find the embedded paths for all node
         pairs with the minimum total cost. The undirected graph is called base graph
         and the node pairs are described as an undirected demand graph.

        There is currently no known algorithm that solves this problem in polynomial time.
        The MCDPP is an integer linear optimization problem, that can be solved
        using a Branch and Bound algorithm.

        You can try the Test instances below, to see a visualization of the MCDPP.
         """)

# Choose test instance
option_simple = st.selectbox(
    'Choose a Test instance:',
    ['1', '2', 'no instance'],
    index = 2
)

base_graph = ''
demand_graph = ''

if option_simple == '1':
    base_graph = 'beispielBesser.gra'
    demand_graph = 'bedarf3.gra'
elif option_simple == '2':
    base_graph = 'klein.gra'
    demand_graph = 'bedarf4.gra'

# Draw test instances
if option_simple != 'no instance':

    # Current directory path
    current_dir = os.path.dirname(os.path.abspath(__file__))

    # Paths to base graph and demand graph files as arguments for solver

    base_graph_path = os.path.join(current_dir, 'Testdateien', base_graph)
    demand_graph_path = os.path.join(current_dir, 'Testdateien', demand_graph)

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

    # Create graphs
    fig_base = plot_graph(base_graph_path, demand_graph_path, sol_path="")
    fig_demand = plot_graph(demand_graph_path, demand_graph_path, sol_path="")
    fig_sol = plot_graph(base_graph_path, demand_graph_path, sol_path="ergebnis_output.txt")
    st.write("Base graph:")
    st.pyplot(fig_base)
    st.write("Demand graph:")
    st.pyplot(fig_demand)
    st.write("Base Graph with embedded solution:")
    st.pyplot(fig_sol)


# Test instance with no solution
st.write("It can be, that the Instance of the MCDPP has no solution")


# Big Test instance
