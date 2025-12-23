import sys

"""
Argument validations throughout the code are left from original HW1 submission 
"""
def is_valid_int(value):
    """
    Check if the given string represents a valid integer.
    Accepts formats like "3", "3.0", "7.000" (but not "3.1").

    Args:
        value (str): String to validate.

    Returns:
        bool: True if the string represents a valid integer, False otherwise.
    """
    for i in range(len(value)):
        if not value[i].isdigit() and value[i] != ".":
            return False
        elif value[i] == ".":
            if i == len(value) - 1:
                return False
            i += 1
            while i < len(value):
                if value[i] != '0':
                    return False
                i += 1
            break
    return True

def main():
    """
    Entry point of the program.

    Parses command-line arguments, validates them,
    and starts the k-means clustering process.

    Raises:
        Prints error message and exits if arguments are invalid.
    """
    if len(sys.argv) not in [2,3]:
        print("An Error Has Occurred")
        sys.exit(1)

    if not is_valid_int(sys.argv[1]):
        print("Incorrect number of clusters!")
        sys.exit(1)

    k = int(float(sys.argv[1]))

    if len(sys.argv) == 3:
        if not is_valid_int(sys.argv[2]):
            print("Incorrect maximum iteration!")
            sys.exit(1)
        max_iter = int(float(sys.argv[2]))
        return k_mean(k, max_iter) 
    else:
        return k_mean(k)  

def k_mean(k, max_iter=400):  
    """
    Runs k-means clustering on input data from stdin.

    Args:
        k (int): number of clusters.
        max_iter (int): maximum number of iterations (default 400).

    Returns:
        list: final centroids after convergence or max iterations.
    
    Raises:
    Prints error and exits if k or max_iter are invalid.

    """
    if not (1 < max_iter < 1000):
        print("Incorrect maximum iteration!")
        sys.exit(1)

    points, dimension, num_points = load_and_validate_data()

    if not (2 <= k < num_points):
        print("Incorrect number of clusters!")
        sys.exit(1)

    centroids = [points[i] for i in range(k)]
    return run_kmeans(points, centroids, dimension, k, max_iter)

def load_and_validate_data():
    """
    Load and validate data points from stdin.
    Each line must contain comma-separated floats of equal length.

    Returns:
        tuple: (points, dimension, num_points)
            - points (list[list[float]]): List of points read from stdin.
            - dimension (int): Dimensionality of each point.
            - num_points (int): Number of points loaded.

    Raises:
        Prints error and exits if data format is invalid.
    """
    points = []
    dimension = None
    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        split_line = line.split(',')
        if dimension is None:
            dimension = len(split_line)
        elif len(split_line) != dimension:
            print("An Error Has Occurred")
            sys.exit(1)
        try:
            point = [float(x) for x in split_line]
        except Exception:
            print("An Error Has Occurred")
            sys.exit(1)

        points.append(point)

    return points, dimension, len(points)

def run_kmeans(points, centroids, dimension, k, max_iter):
    """
    Perform the iterative k-means algorithm.

    Args:
        points (list[list[float]]): Data points.
        centroids (list[list[float]]): Initial centroids.
        dimension (int): Number of features per point.
        k (int): Number of clusters.
        max_iter (int): Maximum iterations.

    Returns:
        list[list[float]]: Final centroids after clustering.
    """
    epsilon = 1e-4
    num_points = len(points)
    iteration_count = 0
    continue_updating = True
    while iteration_count < max_iter and continue_updating:
        continue_updating = False
        iteration_count += 1
        clusters = [[] for _ in range(k)]
        # assign points to closest centroid
        for i in range(num_points):
            closest_idx = closest_cluster(points[i], centroids)
            clusters[closest_idx].append(points[i])
        # update centroids
        for i in range(k):
            cluster_points = clusters[i]
            cluster_size = len(cluster_points)
            new_centroid = [0.0] * dimension
            for j in range(dimension):
                for point in cluster_points:
                    new_centroid[j] += point[j]
                new_centroid[j] /= cluster_size
            if distance(centroids[i], new_centroid) >= epsilon: #check if centroid has moved significantly
                continue_updating = True
            centroids[i] = new_centroid
    return centroids # Originally were printed

def distance(p, q):
    """
    Computes the Euclidean distance between two points.
    
    Args:
        x (list): coordinates of the first point.
        y (list): coordinates of the second point.

    Returns:
        float: Euclidean distance between x and y.
    """
    return sum((p[i] - q[i]) ** 2 for i in range(len(p))) ** 0.5

def closest_cluster(point, centroids):
    """
    Find the closest centroid to a given point.

    Args:
        point (list[float]): Data point.
        centroids (list[list[float]]): List of centroids.

    Returns:
        int: Index of the closest centroid.
    """
    min_dist = float('inf')
    closest_idx = 0
    for i in range(len(centroids)):
        dist = distance(point, centroids[i])
        if dist < min_dist:
            min_dist = dist
            closest_idx = i
    return closest_idx

if __name__ == "__main__":
    main()