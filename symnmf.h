#ifndef SYMNMF_H
#define SYMNMF_H

#include <stdlib.h>

/* Function prototypes */
/* Core API functions */
double** sym(double **data, int N, int d);
double** ddg(int N, double **A);
double** norm (int N, double **D, double **A);
void symnmf(double **curr_H, double **W, int N, int k); 
/* Memory management functions */
void free_matrix_memory(double **matrix, int rows);
void allocate_matrix_memory(double ***matrix, int N, int k);


#endif /* SYMNMF_H */