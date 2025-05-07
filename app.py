import streamlit as st
from MCDPPExecution.execute_MCDPP import execute_MCDPP_solver, plot_graph, extract_solution
import os
import matplotlib.pyplot as plt
from PIL import Image


# TODO: Implement side bar with: MCDPP, MCDPP Solver, Create own MCDPP instance
# TODO: Styling of the website

st.sidebar.header("MCDPP Solver")
st.sidebar.divider()
option = st.sidebar.radio(
    "Sections",
    ["MCDPP description","Solver options" ,"MCDPP Solver"]
)


# =========================
# Description of the MCDPP
# ========================
if option == "MCDPP description":

    st.title("Minimum Cost Disjoint Path Problem (MCDPP)")

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


# ====================================
# Description of format for the graphs
# ====================================

if option == "Solver options":
    st.title("Solver options")

    st.markdown("## Format of the graphs")
    st.write("""
            The base graph and the demand graph are both undirected graphs. They are
            saved in a .gra file. In this format # is used to comment a line and
            empty lines are ignored. The first line of the file contains the number of
            nodes. The second line contains the number of edges. The third line contains
            information about the direction. Due to undirected graphs this line contains
            only the word "ungerichtet".

            The next lines contain the nodes of the graph in the following sequence:
            1. node id (unique string or number)
            2. x coordinate of the node
            3. y coordinate of the node

            In the last lines the edges of the graph are described in the following sequence:
            1. edge id (unique string or number)
            2. node id of the first node of the edge
            3. node id of the second node of the edge
            4. cost of the edge (non-negative integer)

            In the case of a demand graph the cost of the edges is ignored and can
            be left empty. Furthermore the node ids of the demand graph are a subset
            of the node ids of the base graph.
            Here is an example of a base graph and a demand graph:
            """)


    st.markdown("### Base graph:")
    content = """
            # Base graph example

            9   # number of nodes
            18   # number of edges
            ungerichtet

            # node_id     x-coord      y-coord

            1            6.5          12.0
            2            6.5          6
            3            6.5          0
            4            15.5         12.0
            5            12.5         6
            6            18.5         6
            7            24.5          12
            8            24.5          6
            9            24.5          0


            # edge_id   first node    second node      cost

            0101            1           2              1
            0102            1           5              300
            0103            1           4              200
            0104            2           4              50
            0105            2           5              80
            0106            2           9              130
            0107            2           3              210
            0108            3           8              110
            0109            3           9              120
            0110            4           5              90
            0111            4           6              10
            0112            4           8              60
            0113            4           7              100
            0114            5           6              30
            0115            6           7              20
            0116            6           8              40
            0117            7           8              70
            0118            8           9              140
            """

    st.code(content, language="text")

    st.markdown("### Demand graph:")
    content = """
            # Demand graph example

            4  # number of nodes
            3  # number of edges
            ungerichtet

            # node_id (from base graph)     x-coord      y-coord

            1            6.5          12.0
            2            6.5          6
            7            24.5          12
            9            24.5          0

            # edge_id    first node     second node

            A      1     7
            B      2     7
            C      2     9

            """

    st.code(content, language="text")
    st.markdown("## Solver parameters")
    st.write("""
             The solver for the MCDPP has the following parameters:
            1. base graph
            2. demand graph
            3. Number of iterations for subgradient calculation (lower bound from Lagrange relaxation)
            4. Number of iterations for the step size update of subgradient calculation (lower bound)
            5. Number of iterations for the subgradient calculation for determination of branch node
                for Branch and Bound procedure (only for option 3 in 7th parameter)
            6. Option for the starting lambda value for subgradient calculation

                1 = set lambda to 0

                2 = set lambda to the value, for which parent node of Branch and Bound has maximum lower bound in subgradient calculation

                3 = set lambda to the value, for which parent node of Branch and Bound has minimum number of conflicts for embedded paths
            7. Option for choosing the next branch node for the Branch and Bound procedure

                1 = choose node in base_graph with the maximum number of conflicts for embedded paths at lambda_option

                2 = choose middle node in base_graph of the embedded demand with the shortest way at lambda_option

                3 = take all conflict nodes of the embedded demands at lambda_option

            Due to the 6. and 7. parameter, there are 9 different solvers for the MCDPP.
            The default values for the parameters are:

            1. base graph
            2. demand graph
            3. 200
            4. 10
            5. 40
            6. 2
            7. 1

            This default solver is suitable for many instances, to solve them in a short time.
            """)




# ====================================
# Implementation of the solver
# ====================================

if option == "MCDPP Solver":
    st.title("MCDPP Solver")

    current_dir = os.path.dirname(os.path.abspath(__file__))

    base_graph_file = st.file_uploader("Upload base graph", type=["gra"])
    demand_graph_file = st.file_uploader("Upload demand graph", type=["gra"])

    # Parameter for visualization
    st.markdown("## Visualization of the graphs")
    fig_size_x = st.slider("width of graph plot picture in inch:", min_value=1, max_value=100, value=20)
    fig_size_y = st.slider("height of graph plot picture in inch:", min_value=1, max_value=100, value=25)
    fig_dpi = st.slider("picture dots per inch:", min_value=1, max_value=1000, value=75)
    node_size = st.slider("size of nodes in graph in pt^2:", min_value=1, max_value=1000, value=500)
    edge_size = st.slider("size of edges in graph in pt:", min_value=1, max_value=100, value=3)
    terminal_size = st.slider("size of demand nodes in pt^2:", min_value=1, max_value=1000, value=500)
    edge_labels = st.checkbox("show edge costs", value=False)
    edge_labels_size = 0
    edge_label_pos = 0
    if edge_labels:
        edge_labels_size = st.slider("size of edge labels in pt:", min_value=1, max_value=100, value=10)
        edge_label_pos = st.slider("position of edge labels in percent of the edge:", min_value=0.0, max_value=1.0, value=0.5)



    # Visualization of the graphs
    save_base_path = os.path.join(current_dir, 'Results', 'base_graph.gra')
    save_demand_path = os.path.join(current_dir, 'Results', 'demand_graph.gra')

    # We need a demand graph for the base graph visualization
    if base_graph_file is not None and demand_graph_file is not None:
        # Save file
        file_contents = base_graph_file.read().decode("utf-8")
        with open(save_base_path, "w") as f:
            f.write(file_contents)

        # plot file
        fig = plot_graph(save_base_path, save_demand_path, sol_path="",
                         fig_size_x=fig_size_x, fig_size_y=fig_size_y, fig_dpi=fig_dpi,
                        node_size=node_size, edge_size=edge_size, terminal_size=terminal_size,
                        edge_labels=edge_labels, edge_lables_size=edge_labels_size,
                        edge_lable_pos=edge_label_pos)
        st.markdown("## Base graph:")
        st.pyplot(fig)

    # Demand graph visualization
    if demand_graph_file is not None:
        # Save file
        file_contents = demand_graph_file.read().decode("utf-8")
        with open(save_demand_path, "w") as f:
            f.write(file_contents)

        # plot file
        fig = plot_graph(save_demand_path, save_demand_path, sol_path="")
        st.markdown("## Demand graph:")
        st.pyplot(fig)

    # Solver attributes
    st.markdown("## Solver parameters")
    n_sub = st.slider("Number of iterations for subgradient calculation (lower bound from Lagrange relaxation)",
                      min_value=10, max_value=1000, value=200)
    n_iter_half = st.slider("Number of iterations for the step size update of subgradient calculation (lower bound)",
                            min_value=3, max_value=1000, value=10)
    n_sub_variant = 40
    lambda_option = st.number_input("Option for the starting lambda value for subgradient calculation",
                                min_value=1, max_value=3, value=2)
    VZK_option = st.number_input("Option for choosing the next branch node for the Branch and Bound procedure",
                            min_value=1, max_value=3, value=1)
    if VZK_option == 3:
        n_sub_variant = st.slider("Number of iterations for the subgradient calculation for determination of branch node for Branch and Bound procedure",
                                min_value=10, max_value=1000, value=40)

    # Solver
    if st.button("Run solver"):
        if base_graph_file is not None and demand_graph_file is not None:
            # Run solver
            execute_MCDPP_solver(save_base_path,
                                 save_demand_path,
                                 n_sub,
                                 n_iter_half,
                                 n_sub_variant,
                                 lambda_option,
                                 VZK_option)

            result_time = -1
            result_costs = -1

            # Does solution exists
            solution_exists = True
            with open("ergebnis_output.txt", 'r') as file:
                for line in file:
                    if line.startswith("Zeit in s:"):
                        result_time = line.split(":")[1].strip()
                    if line.startswith("MCDPP besitzt keine Loesung."):
                        solution_exists = False
                        break

            # Create graphs, when solution exists
            st.markdown("## Result:")
            st.write(f"Time of the solver in s: {result_time}")

            if solution_exists:
                # costs of optimal solution
                result = extract_solution('ergebnis_output.txt')
                result_costs = result['total_cost']
                st.write(f"Total cost of the solution: {result_costs}")

                #plot solution
                fig = plot_graph(save_base_path, save_demand_path, sol_path="ergebnis_output.txt",
                                fig_size_x=fig_size_x, fig_size_y=fig_size_y, fig_dpi=fig_dpi,
                                node_size=node_size, edge_size=edge_size, terminal_size=terminal_size,
                                edge_labels=edge_labels, edge_lables_size=edge_labels_size,
                                edge_lable_pos=edge_label_pos)
                st.markdown("## Base graph with embedded solution:")
                st.pyplot(fig)

            else:
                st.write("No solution exists for this instance.")
        else:
            st.warning("Please upload both base graph and demand graph.")

    # Run solver
    # execute_MCDPP_solver(base_graph_path,
    #                     demand_graph_path,
    #                     n_sub,
    #                     n_iter_half,
    #                     n_sub_variant,
    #                     lambda_option,
    #                     VZK_option)




    #  # Current directory path
    # current_dir = os.path.dirname(os.path.abspath(__file__))
    # parent_dir = os.path.join(current_dir, os.pardir)


    # # Paths to base graph and demand graph files as arguments for solver
    # base_graph = 'beispielBesser.gra'
    # demand_graph = 'bedarf3ohneKap.gra'
    # base_graph_path = os.path.join(parent_dir, 'Testdateien', base_graph)
    # demand_graph_path = os.path.join(parent_dir, 'Testdateien', demand_graph)

    # # Solver attributes
    # n_sub = 200
    # n_iter_half = 10
    # n_sub_variant = 40
    # lambda_option = 2
    # VZK_option = 1

    # # Run solver
    # execute_MCDPP_solver(base_graph_path,
    #                     demand_graph_path,
    #                     n_sub,
    #                     n_iter_half,
    #                     n_sub_variant,
    #                     lambda_option,
    #                     VZK_option)

    # # Does solution exists
    # solution_exists = True
    # with open("ergebnis_output.txt", 'r') as file:
    #     for line in file:
    #         if line.startswith("MCDPP besitzt keine Loesung."):
    #             solution_exists = False
    #             break

    # # Create graphs, when solution exists
    # if solution_exists:
    #     fig = plot_graph(base_graph_path, demand_graph_path, sol_path="ergebnis_output.txt",
    #                     edge_labels=True, edge_lables_size=30, edge_lable_pos=0.3)
    #     fig.savefig("graph.jpg", format="jpg")





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
