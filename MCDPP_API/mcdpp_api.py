from fastapi import FastAPI, UploadFile, File
import subprocess
import os


# TODO: create and test a docker image
# TODO: Docker an gcp schicken + timeout option beachten + Dauer der Berechnung auf Server beachten
# TODO: Streamlit App API Call für Solver erstellen
# TODO: Webseite in Lebenslauf packen wie AI Tutor
# TODO: troopl Projekt Webseite hinzufügen



# Create Fast API
app = FastAPI()

# Upload directory for base graph and demand graph
CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
UPLOAD_DIR = os.path.join(CURRENT_DIR, "uploads")
base_graph_path = os.path.join(UPLOAD_DIR, 'base_graph.gra')
demand_graph_path = os.path.join(UPLOAD_DIR, 'demand_graph.gra')


# Creating Endpoint
@app.get("/")
def root():
    return {
    'greeting': 'This is an API for execting MCDPP'
}


# Upload base graph
@app.post("/upload_base_gra/")
async def upload_base_gra_file(file: UploadFile = File(...)):
    """
    Endpoint to upload a .gra file (txt format) and save it as base_graph.gra.
    """
    try:
        # Ensure the file is a .gra file
        if not file.filename.endswith(".gra"):
            return {"error": "Only .gra files are allowed."}

        # Read the file content as bytes
        content = await file.read()

        # Decode bytes to a string
        text_content = content.decode("utf-8")
        with open(base_graph_path, "w", encoding="utf-8") as output_file:
            output_file.write(text_content)

        return {"message": f"File '{file.filename}' saved successfully as 'base_graph.gra'"}

    except Exception as e:
        return {"error": str(e)}


# Upload demand graph
@app.post("/upload_demand_gra/")
async def upload_demand_gra_file(file: UploadFile = File(...)):
    """
    Endpoint to upload a .gra file (txt format) and save it as demand_graph.gra.
    """
    try:
        # Ensure the file is a .gra file
        if not file.filename.endswith(".gra"):
            return {"error": "Only .gra files are allowed."}

        # Read the file content as bytes
        content = await file.read()

        # Decode bytes to a string
        text_content = content.decode("utf-8")
        with open(demand_graph_path, "w", encoding="utf-8") as output_file:
            output_file.write(text_content)

        return {"message": f"File '{file.filename}' saved successfully as 'demand_graph.gra'"}

    except Exception as e:
        return {"error": str(e)}


# execute MCDPP
@app.get("/execute_MCDPP")
def execute_MCDPP(
    n_sub: int,
    n_iter_half: int,
    n_sub_variant: int,
    lambda_option: int,
    VZK_option: int
):
    """
    This function executes the MCDPP solver with specified parameters:
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
    the solver in ergebnis_output.txt file. From that a dictionary is created
    with the following keys:
    - time: Time of the solver in seconds
    - total_cost: Total cost of the solution
    - paths: List of paths in the solution, where each path is a list of edge indices
    """



    # Path to the .exe file for solving Minimum Cost Disjoint Path Problem (MCDPP)
    exe_path = os.path.join(CURRENT_DIR, './mcdpp')

    # Erase content of the output file
    with open('ergebnis_output.txt', "w") as file:
        file.write("")

    # Execute solver
    subprocess.run([exe_path,
                    str(base_graph_path),
                    str(demand_graph_path),
                    str(n_sub),
                    str(n_iter_half),
                    str(n_sub_variant),
                    str(lambda_option),
                    str(VZK_option)],
                   check=True)

    # Read the output file
    output_file_path = os.path.join(CURRENT_DIR, 'ergebnis_output.txt')
    result = ""

    try:
        with open(output_file_path, 'r') as file:
            result = file.read()
    except FileNotFoundError:
        result = "Output file not found. Please check the solver execution."

    # Turn result into a dictionary
    result_dict = {}
    result_dict["time"] = -1
    result_dict["total_cost"] = -1
    result_dict["paths"] = []
    paths = []
    counter = 0

    if result != "" and result != "Output file not found. Please check the solver execution.":
        lines = result.split('\n')
        for line in lines:
            if line.startswith("MCDPP besitzt keine Loesung."):
                # MCDPP has no solution
                result_dict["total_cost"] = -1
                result_dict["paths"] = []
                return result_dict
            elif line.strip() == "":
                # skip empty lines
                continue
            elif counter == 0:
                # read time of the solver
                result_dict["time"] = float(line.strip().split()[-1])
                counter += 1
            elif counter == 1:
                # read total cost
                result_dict["total_cost"] = float(line.strip().split()[-1])
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
    result_dict['paths'] = paths

    return result_dict
