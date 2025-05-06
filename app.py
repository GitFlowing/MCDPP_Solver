import streamlit as st
from MCDPPExecution.execute_MCDPP import execute_MCDPP_solver, plot_graph
import os
import matplotlib.pyplot as plt
from PIL import Image


# TODO: Implement side bar with: MCDPP, MCDPP Solver, Create own MCDPP instance
# TODO: Styling of the website

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
    ['1', '2']
)

# Show images of small instances
current_dir = os.path.dirname(os.path.abspath(__file__))
base_graph_path = ""
demand_graph_path = ""
solution_graph_path = ""

if option_simple == '1':
    base_graph_path = os.path.join(current_dir, 'graph_pics', 'winzig_base.jpg')
    demand_graph_path = os.path.join(current_dir, 'graph_pics', 'winzig_demand.jpg')
    solution_graph_path = os.path.join(current_dir, 'graph_pics', 'winzig_sol.jpg')

if option_simple == '2':
    base_graph_path = os.path.join(current_dir, 'graph_pics', 'klein_base.jpg')
    demand_graph_path = os.path.join(current_dir, 'graph_pics', 'klein_demand.jpg')
    solution_graph_path = os.path.join(current_dir, 'graph_pics', 'klein_sol.jpg')

# Display image
image_base = Image.open(base_graph_path)
image_demand = Image.open(demand_graph_path)
image_solution = Image.open(solution_graph_path)

st.image(image_base, caption='Base graph', use_container_width =True)
st.image(image_demand, caption='Demand graph', use_container_width =True)
st.image(image_solution, caption='Embedded solution', use_container_width =True)



# Test instance with no solution
st.write("""It can be, that the Instance of the MCDPP has no solution.
         When you delete 3 edges in test instance 1 from before, there is no solution.""")

base_graph_path = os.path.join(current_dir, 'graph_pics', 'winzig_unloesbar.jpg')
demand_graph_path = os.path.join(current_dir, 'graph_pics', 'winzig_demand.jpg')

image_base = Image.open(base_graph_path)
image_demand = Image.open(demand_graph_path)

st.image(image_base, caption='Base graph', use_container_width =True)
st.image(image_demand, caption='Demand graph', use_container_width =True)

# Big Test instance
st.write("""Things are getting complicated, if the number of nodes and edges increases.
         A real world example could be the leasing of a number of connections in a
         telecommunication network in Germany. The base graph is the telecommunication network
         and the demand graph describes the connections that are to be leased.
         The goal is to find an embedding of the connections in the network with the
         minimum costs and without crossing each other. If the crossing of connections
         were permitted, several connections would fail if one connection failed
         at the crossing point. This must therefore be avoided. Therefore this
         real world problem can be described as the MCDPP, which can take time
         to be solved.""")

base_graph_path = os.path.join(current_dir, 'graph_pics', 'Testinstanz4Base.png')
demand_graph_path = os.path.join(current_dir, 'graph_pics', 'Testinstanz4Demand.png')
solution_graph_path = os.path.join(current_dir, 'graph_pics', 'Testinstanz4Optimum.png')

image_base = Image.open(base_graph_path)
image_demand = Image.open(demand_graph_path)
image_solution = Image.open(solution_graph_path)

st.image(image_base, caption='Base graph', use_container_width =True)
st.image(image_demand, caption='Demand graph', use_container_width =True)
st.image(image_solution, caption='Embedded solution', use_container_width =True)

st.write("""The base graph of this test instance has 676 nodes and 1107 edges.
         In total 19 demands should be embedded in this base graph. Due to these
         high numbers, the costs of the edges are not displayed in the base graph
         and some nodes are overlapping. The cost of every edge lays between 1 and 215.
         The embedded solution has a total cost of 7442.
         Depending on the instance the time of the solver can vary a lot.""")



# Draw test instances
# if option_simple != 'no instance':

#     # Paths to base graph and demand graph files as arguments for solver

#     base_graph_path = os.path.join(current_dir, 'Testdateien', base_graph)
#     demand_graph_path = os.path.join(current_dir, 'Testdateien', demand_graph)

#     # Solver attributes
#     n_sub = 200
#     n_iter_half = 10
#     n_sub_variant = 40
#     lambda_option = 2
#     VZK_option = 1

#     # Run solver
#     execute_MCDPP_solver(base_graph_path,
#                         demand_graph_path,
#                         n_sub,
#                         n_iter_half,
#                         n_sub_variant,
#                         lambda_option,
#                         VZK_option)

#     # Create graphs
#     fig_base = plot_graph(base_graph_path, demand_graph_path, sol_path="")
#     fig_demand = plot_graph(demand_graph_path, demand_graph_path, sol_path="")
#     fig_sol = plot_graph(base_graph_path, demand_graph_path, sol_path="ergebnis_output.txt")
#     st.write("Base graph:")
#     st.pyplot(fig_base)
#     st.write("Demand graph:")
#     st.pyplot(fig_demand)
#     st.write("Base Graph with embedded solution:")
#     st.pyplot(fig_sol)
