import subprocess
import os

# Current directory path
current_dir = os.path.dirname(os.path.abspath(__file__))

# Path to the .exe file for solving Minimum Cost Disjoint Path Problem (MCDPP)
exe_path = os.path.join(current_dir, 'MCDPP.exe')

# Paths to base graph and demand graph files as arguments for solver
base_graph = 'deutschland1.gra'
demand_graph = 'bedarf19.gra'
base_graph_path = os.path.join(current_dir, 'Testdateien', base_graph)
demand_graph_path = os.path.join(current_dir, 'Testdateien', demand_graph)

# Solver attributes
n_sub = 200
n_iter_half = 10
n_sub_variant = 40
lambda_option = 2
VZK_option = 1

# Execute solver
result = subprocess.run([exe_path,
                         base_graph_path,
                         demand_graph_path,
                         str(n_sub),
                         str(n_iter_half),
                         str(n_sub_variant),
                         str(lambda_option),
                         str(VZK_option)])
content = ""

# Result of solver
#with open("output.txt", "r") as file:
#    content = file.read()





# Starte das Programm und fange die Ausgabe ab
# process = subprocess.Popen(
#     [exe_path, base_graph_path, demand_graph_path],  # Das Programm, das du ausführen möchtest
#     stdout=subprocess.PIPE,  # Um die Ausgabe zu erfassen
#     stderr=subprocess.PIPE,  # Um Fehlerausgaben zu erfassen (optional)
#     text=True  # Gibt die Ausgabe als String zurück (nicht als Bytes)
# )

# # Lese die Standardausgabe (stdout)
# output, error = process.communicate()  # Warten bis das Programm beendet ist

# # Zeige die Ausgabe an
# print("Programm-Ausgabe:")
# print(output)
# print(type(output))

# # Optional: Fehlerausgabe anzeigen, falls vorhanden
# if error:
#     print("Fehler-Ausgabe:")
#     print(error)
