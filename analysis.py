import sys
import sklearn.metrics as skm
import symnmf
import symnmf_ext
import kmeans
import numpy as np

def read_cmd_args():
    """
    Parse the command-line input and extract k and the data file name.

    Returns:
        tuple: (k, file_name)
            - k (int): number of clusters requested.
            - file_name (str): path to the dataset file.

    Raises error:
        Prints an error and stops the program if arguments are missing or excessive.
    """
    if len(sys.argv) != 3:
        print("An Error Has Occurred")
        sys.exit(1)
    k = int(sys.argv[1]) 
    file_name = sys.argv[2]
    return k, file_name

def euclidean_distance(x, y):
    """
    Computes the Euclidean distance between two points.
    
    Args:
        x (list): coordinates of the first point.
        y (list): coordinates of the second point.

    Returns:
        float: Euclidean distance between x and y.
    """
    return np.linalg.norm(np.array(x) - np.array(y))

def process_symnmf_result(H):
    """
    Convert the factorization output into cluster labels.

    Args:
        H (list): factorized matrix from SymNMF.

    Returns:
        list: cluster index for each row, chosen by the maximum entry.
    """
    return [np.argmax(row) for row in H]

def assign_kmeans_clusters(points, centroids):
    """
    Assigns each point to the closest KMeans cluster based on Euclidean distance.

    Args:
        points (list): dataset points.
        centroids (list): cluster centers from KMeans.

    Returns:
        list: cluster id assigned to each point.
    """
    return [np.argmin([euclidean_distance(point, centroid) for centroid in centroids]) for point in points]

def main():
    # Read cmd arguments and create matrix of points
    k, filename = read_cmd_args()
    data = np.loadtxt(filename, delimiter=",", dtype=np.float64)
    data = data.tolist()

    # Create normalized matrix, initialize H, and run SymNMF
    normalized_matrix = symnmf_ext.norm(data)
    initial_h = symnmf.initialize_h_matrix(k, normalized_matrix).tolist()
    symnmf_clustering = process_symnmf_result(symnmf_ext.symnmf(normalized_matrix, initial_h, k))

    original_argv = sys.argv  # Backup original arguments
    sys.argv = ["kmeans.py", str(k), "300"]   # Set k, max_iter = 300 for kmeans.py 
    file = open(filename) # Make sure filename is passed as input for stdin (like original kmeans from hw1)
    sys.stdin = file

    # Run KMeans and assign points to clusters
    kmeans_clustering = kmeans.main()
    kmeans_clustering = assign_kmeans_clusters(data, kmeans_clustering)
    
    file.close()
    sys.argv = original_argv # Restore original sys.argv
    sys.stdin = sys.__stdin__ # Restore original stdin

    # Compute silhouette scores 
    kmeans_sil = skm.silhouette_score(data, kmeans_clustering)
    nmf_sil = skm.silhouette_score(data, symnmf_clustering)

    # Print silhouette scores
    print(f"nmf: {format(nmf_sil, '.4f')}")
    print(f"kmeans: {format(kmeans_sil, '.4f')}")

if __name__ == "__main__":
    main()