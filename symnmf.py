import numpy as np
import sys 
import symnmf_ext
import math 

np.random.seed(1234) # Keep results consistent for testing

def main():
    
    # Read command line arguments
    k, goal, file_name = read_cmd_args() 
    
    # Load data from file and convert to matrix
    data = np.loadtxt(file_name, delimiter=",", dtype=np.float64)
    data = data.tolist()

    # Run function based on wanted goal
    if goal == "sym":
        result = symnmf_ext.sym(data)  # Compute similarity matrix A
    elif goal == "ddg":
        result = symnmf_ext.ddg(data)  # Compute diagonal degree matrix D
    elif goal == "norm":
        result = symnmf_ext.norm(data)  # Compute normalized similarity matrix W
    elif goal == "symnmf":
        norm = symnmf_ext.norm(data)
        initial_h = (initialize_h_matrix(k, norm)).tolist()
        result = symnmf_ext.symnmf(norm, initial_h, k)  # Run full SymNMF algorithm
    else:
        print("An Error Has Occurred")
        sys.exit(1)

    #Print the output matrix, formatted to 4 decimal places
    for row in result:
        formatted = ["%.4f" % num for num in row]
        print(*formatted, sep=",")

def read_cmd_args():
    """
    Reads command line arguments and returns the values of k, goal, and file_name.
    
    Returns:
        tuple: (k, goal, file_name)
            - k (int): The number of clusters to create.
            - goal (str): Program's goal.
            - file_name (str): Input data file.
    
    Raises error:
        SystemExit: If arguments are missing or incorrect.
    """
    if len(sys.argv) != 4:
        print("An Error Has Occurred")
        sys.exit(1)
    k = int(sys.argv[1])
    goal = sys.argv[2]
    file_name = sys.argv[3]
    return k, goal, file_name

def initialize_h_matrix(k, normalized):
    """
    Initializes matrix H with random values in range 0 to 2*sqrt(avg/k). 'avg' is the mean of 'normalized' (W).
    The size is (number of rows in 'result') x k.     
    
    Args:
        k (int): The number of clusters to create (columns in H).
        normalized (np.ndarray): The normalized similarity matrix.
    
    Returns:
        np.ndarray: A random matrix of shape (rows, k) with values uniform(0, 2*sqrt(avg/k)).
    """
    # Average of the normalized matrix and range of the random matrix values
    avg = np.mean(normalized)
    value_range = 2 * math.sqrt(avg / k)

    # Create the matrix with random values
    rows = len(normalized)
    h_matrix = np.random.uniform(0, value_range, (rows, k))
    return h_matrix

if (__name__ == "__main__"):
    main()