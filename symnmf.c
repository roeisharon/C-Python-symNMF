#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "symnmf.h"

/* Constants */
#define int_INFINITY 2147483647
#define EPSILON 1e-4
#define SMALL_EPSILON 1e-12
#define MAX_ITER 300
#define BETA 0.5

/*Function Declarations*/
/* Math / matrix utilities */
void compute_root(double **D, int N, double *D_root);
void transpose_matrix(double **mat, double **transposed_mat, int N, int k);
void multiply_matrix(double **mat1, double **mat2, double **result, int n, int m, int p);
void sub_matrix(double **mat1, double **mat2, double **result, int N, int M);
double frobenius_norm(double **mat, int N, int M);
/* File utilities */
void analyze_matrix_dims(const char* filename, int* N, int* d);
double** file_to_matrix(const char* filename, int N, int d);
void print_matrix(double** matrix, int N, int d);

int main(int argc, char *argv[]) {
    char *goal, *filename;
    double **points, **A, **D, **W;
    int N = 0, d = 0;
    if (argc != 3) { /* Make sure correct number of arguments and extract */
        printf("An Error Has Occurred\n");
        return 1;
    }
    goal = argv[1]; 
    filename = argv[2];
    /* Analyze the matrix dimensions from file and convert it into matrix */
    analyze_matrix_dims(filename, &N, &d);
    points = file_to_matrix(filename, N, d);
    if (strcmp(goal, "sym") == 0){ /* Validate goal */
        A = sym(points, N, d); /* Compute similarity matrix */
        print_matrix(A, N, N); /* Print matrix */
    } 
    else if (strcmp(goal, "ddg") == 0){
        A = sym(points, N, d); /* Compute similarity matrix */
        D = ddg(N, A); /* Compute diagonal degree matrix */
        print_matrix(D, N, N); /* Print matrix */
        free_matrix_memory(D, N); /* Free memory */
    } 
    else if (strcmp(goal, "norm") == 0){
        A = sym(points, N, d); /* Compute similarity matrix */
        D = ddg(N, A); /* Compute diagonal degree matrix */
        W = norm(N, D, A); /* Compute norm matrix */
        print_matrix(W, N, N); /* Print matrix */
        free_matrix_memory(D, N); /* Free memory */
        free_matrix_memory(W, N); /* Free memory */
    }
    else { 
        printf("An Error Has Occurred\n"); 
        return 1;
    }
    free_matrix_memory(A, N); /* If we are here we allocated memory for using at least matrix A */
    free_matrix_memory(points, N); /* Free rest of memory */
    return 0;
}

double** sym(double **data, int N, int d) {
    /* Creates the similarity matrix A from input*/
    int i, j, k;
    double sum, diff;
    double **A;

    /* Allocate NxN matrix for similarity */
    allocate_matrix_memory(&A, N, N);

    /* Calculate similarity between every two points */
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (i == j) {
                A[i][j] = 0.0; /* Zeros at diagonal */
            } else {
                sum = 0.0;
                for (k = 0; k < d; k++) {
                    diff = data[i][k] - data[j][k]; /* Difference between coordinates */
                    sum += diff * diff; /* Sum the square of differences */
                }
                A[i][j] = exp(-sum / 2.0); /* Gaussian similarity score */
            }
        }
    }
    return A;
}

/* Compute diagonal degree matrix D for the similarity matrix A */
double** ddg (int N, double **A){
    int i, j;
    double sum = 0.0;
    double **D;

    /* Allocate memory for diagonal degree matrix */
    allocate_matrix_memory(&D, N, N);

    /* Compute the diagonal degree matrix */
    for (i = 0; i < N; i++){
        for (j = 0; j < N; j++){
            sum += A[i][j];
            D[i][j] = 0; /* Initialize D to zeros */
        }
        D[i][i] = sum; /* Assign the sum to diagonal */
        sum = 0;
    }

    return D;
}

/* Compute the normalized matrix W for similarity matrix A and ddg matrix D */
double** norm (int N, double **D, double **A){
    int i, j;
    double *D_root;
    double **W;

    /* Allocate memory for norm matrix */
    allocate_matrix_memory(&W, N, N);

    /* Allocate memory for the root matrix */
    D_root = (double*)malloc(N * sizeof(double));

    /* Compute square root of diagonal matrix D */
    compute_root(D, N, D_root);

    /* Compute norm matrix */
    for (i = 0; i < N; i++){
        for (j = 0; j < N; j++){
            W[i][j] = D_root[i] * A[i][j] * D_root[j];
        }
    }

    /* Free memory */
    free(D_root);

    return W;
}

/* Compute square root of diagonal matrix D */
void compute_root (double **D, int N, double *D_root){
    int i;
    for (i = 0; i < N; i++){ /* Compute square root of diagonal matrix D */
        /*Don't likely to happen but just in case to avoid division by zero */
        if (sqrt(D[i][i]) == 0){
            D_root[i] = 1.0 / (sqrt(D[i][i] + SMALL_EPSILON)); 
        }
        else{
            D_root[i] = 1.0 / sqrt(D[i][i]);
        }
    }
/* Store it in 1D array because this is just the diagonal */
}

/* Matrix transpose */
void transpose_matrix(double **mat, double **transposed_mat, int N, int k){
    int i, j; /* transposed_mat is already initialized */
    for (i = 0; i < N; i++){
        for (j = 0; j < k; j++){
            transposed_mat[j][i] = mat[i][j];
        }
    }
}

/* Matrix multiplication */
void multiply_matrix(double **mat1, double **mat2, double **result, int n, int m, int p) {
    int i, j, k;
    for (i = 0; i < n; i++) {
        for (j = 0; j < p; j++) {
            result[i][j] = 0.0; 
            for (k = 0; k < m; k++) {
                result[i][j] += mat1[i][k] * mat2[k][j];  
            }
        }
    }
}

/* Substraction between 2 matrices */
void sub_matrix(double **mat1, double **mat2, double **result, int N, int M){
    int i, j;
    for (i = 0; i < N; i++){
        for (j = 0; j < M; j++){
            result[i][j] = mat1[i][j] - mat2[i][j];
        }
    }
}

/* Compute squared Frobenius norm of matrix */
double frobenius_norm(double **mat, int N, int M){
    int i, j;
    double sum = 0.0;
    for (i = 0; i < N; i++){ 
        for (j = 0; j < M; j++){
            sum += mat[i][j] * mat[i][j]; 
        }
    }
    return sum; 
}

/* Free memory allocated for matrix */
void free_matrix_memory(double **matrix, int rows) {
    int i;
    for (i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

/* Allocate memory for matrix */
void allocate_matrix_memory(double ***matrix, int N, int k) {
    int i;
    *matrix = (double**)malloc(N * sizeof(double*));
    if (*matrix == NULL) {
        printf("An Error Has Occurred\n");
        exit(1);
    }
    
    for (i = 0; i < N; i++) {
        (*matrix)[i] = (double*)malloc(k * sizeof(double));
        if ((*matrix)[i] == NULL) {
            printf("An Error Has Occurred\n");
            exit(1);
        }
    }
}

/* Perform the full Symmetric Non-negative Matrix Factorization */
void symnmf(double **curr_H, double **W, int N, int k){
    int i, j, iter;
    double **H_tplus, **W_dot_Ht, **H_transpose_H, **denominator, **H_transpose, **matrix_convergence_check; 
    allocate_matrix_memory(&H_tplus, N, k);
    allocate_matrix_memory(&W_dot_Ht, N, k);
    allocate_matrix_memory(&H_transpose_H, k, k);
    allocate_matrix_memory(&denominator, N, k);
    allocate_matrix_memory(&H_transpose, k, N);
    allocate_matrix_memory(&matrix_convergence_check, N, k);
    for (iter = 0; iter < MAX_ITER; iter++){  
        transpose_matrix(curr_H, H_transpose, N, k);
        multiply_matrix(W, curr_H, W_dot_Ht, N, N, k); 
        multiply_matrix(H_transpose, curr_H, H_transpose_H, k, N, k);
        multiply_matrix(curr_H, H_transpose_H, denominator, N, k, k); 
        for (i = 0; i < N; i++){
            for (j = 0; j < k; j++){
                if (denominator[i][j] == 0.0){
                    H_tplus[i][j] = curr_H[i][j] * (1 - BETA + BETA * (W_dot_Ht[i][j] / (denominator[i][j] + SMALL_EPSILON))); /*Add very small epsilon to avoid division by zero as Yael explained in forum*/
                }
                else{
                    H_tplus[i][j] = curr_H[i][j] * (1 - BETA + BETA * (W_dot_Ht[i][j] / denominator[i][j]));
                }
            }
        }
        sub_matrix(H_tplus, curr_H, matrix_convergence_check, N, k); 
        for (i = 0; i < N; i++){
            for (j = 0; j < k; j++){
                curr_H[i][j] = H_tplus[i][j];
            }
        }
        if (frobenius_norm(matrix_convergence_check, N, k) < EPSILON){
            break;
        }
    }
    free_matrix_memory(H_tplus, N);
    free_matrix_memory(W_dot_Ht, N);
    free_matrix_memory(H_transpose_H, k);
    free_matrix_memory(denominator, N);
    free_matrix_memory(H_transpose, k);
    free_matrix_memory(matrix_convergence_check, N);
}

/* Analyze the file to get number of points and their dimension */
void analyze_matrix_dims(const char* filename, int* N, int* d) {
    FILE* file = fopen(filename, "r"); /*Open file for reading*/
    char line[1024]; /*Buffer of each line*/
    char* token;

    if (!file) { /*Assert file is open*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    /*Read first line*/
    if (fgets(line, sizeof(line), file) == NULL) { 
        printf("An Error Has Occurred\n");
        fclose(file);
        exit(1);
    }

    /*Split line by commas to find the dimension*/
    token = strtok(line, ",");
    while (token) {
        (*d)++;
        token = strtok(NULL, ",");
    }
    (*N) = 1;
    
    /*Count remaining rows (first already included)*/
    while (fgets(line, sizeof(line), file)) {
        (*N)++;
    }

    fclose(file);
}

/* Read and process file into 2D matrix */
double** file_to_matrix(const char* filename, int N, int d) {
    FILE* file = fopen(filename, "r"); /*Open file for reading*/
    char line[1024]; /*Buffer of each line*/
    double** matrix;
    int row = 0, col;
    char* token; 
    if (!file) {
        printf("An Error Has Occurred\n");
        exit(1);
    }
    matrix = (double**)malloc(N * sizeof(double*)); /* Allocate memory for matrix */
    if (!matrix) {
        printf("An Error Has Occurred\n");
        fclose(file);
        exit(1);
    }
    for (row = 0; row < N; row++) {
        matrix[row] = (double*)malloc(d * sizeof(double));
        if (!matrix[row]) {
            printf("An Error Has Occurred\n");
            fclose(file);
            exit(1);
        }
    }
    /* Read file into matrix */
    row = 0;
    while (fgets(line, sizeof(line), file)) {
        col = 0;
        token = strtok(line, ",");
        while (token) {
            matrix[row][col] = atof(token);
            token = strtok(NULL, ",");
            col++;
        }
        row++;
    }
    fclose(file);
    return matrix;
}

/* Print a 2D matrix */
void print_matrix(double** matrix, int N, int d) {
    int i, j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < d; j++) {
            printf("%.4f", matrix[i][j]);
            if (j < d - 1) {
                printf(",");
            }
        }
        printf("\n");
    }
}
