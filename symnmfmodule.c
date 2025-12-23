#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include "symnmf.h"


double** pylist_to_carray(PyObject *pylist, int rows, int cols) {
    double **carray = (double **)malloc(rows * sizeof(double *));
    int j, i;
    PyObject *row, *item;
    if (carray == NULL) {
        /*printf("An Error Has Occurred\n");*/
        return NULL;
    }
    for ( i = 0; i < rows; i++) {
        row = PyList_GetItem(pylist, i);
        carray[i] = (double *)malloc(cols * sizeof(double));
        if (carray[i] == NULL) {
            /*printf("An Error Has Occurred\n");*/
            for (j = 0; j < i; j++) free(carray[j]);
            free(carray);
            return NULL;
        }
        for (int j = 0; j < cols; j++) {
            item = PyList_GetItem(row, j);
            carray[i][j] = PyFloat_AsDouble(item);
        }
    }
    return carray;
}

PyObject* carray_to_pylist(double** matrix, int rows, int cols) {
    PyObject *result = PyList_New(rows);
    PyObject *row;
    int i, j;
    for (i = 0; i < rows; i++) {
        row = PyList_New(cols);
        for (j = 0; j < cols; j++) {
            PyList_SetItem(row, j, PyFloat_FromDouble(matrix[i][j]));
        }
        PyList_SetItem(result, i, row);
    }
    return result;
}

static PyObject* py_sym(PyObject *self, PyObject *args) {
    (void)self;  /* Prevent unused parameter warning */
    PyObject *input_list;
    int N, d;
    double **data, **A;
    PyObject *result;

    /* Parse Python arguments */
    if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &input_list)) {
        return NULL;
    }
    
    N = PyList_Size(input_list);
    d = PyList_Size(PyList_GetItem(input_list, 0));
    
    /* Convert Python list to C array */
    data = pylist_to_carray(input_list, N, d);
    if (data == NULL) return NULL;

    /* Compute similarity matrix */
    A = sym(data, N, d);
    
    /* Convert C array back to Python list */
    result = carray_to_pylist(A, N, N);
    
    /* Free memory */
    free_matrix_memory(A, N);
    free_matrix_memory(data, N);
    
    return result;
}

static PyObject* py_ddg(PyObject *self, PyObject *args) {
    (void)self;  /* Prevent unused parameter warning */
    PyObject *input_list;
    int N, d;
    double **data, **D, **A;
    PyObject *result;

    /* Parse Python arguments */
    if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &input_list)) {
        return NULL;
    }
    
    N = PyList_Size(input_list);
    d = PyList_Size(PyList_GetItem(input_list, 0));
    
    /* Convert Python list to C array */
    data = pylist_to_carray(input_list, N, d);
    if (data == NULL) return NULL;
    
    /* Compute ddg matrix (using the similarity matrix) */
    A = sym(data, N, d);
    D = ddg(N, A);
    
    /* Convert C array back to Python list */
    result = carray_to_pylist(D, N, N);
    
    /* Free memory */
    free_matrix_memory(D, N);
    free_matrix_memory(A, N);
    free_matrix_memory(data, N);
    
    return result;
}

static PyObject* py_norm(PyObject *self, PyObject *args) {
    (void)self;  /* Prevent unused parameter warning */
    PyObject *input_list;
    int N, d;
    double **data, **W, **D, **A;
    PyObject *result;

    /* Parse Python arguments */
    if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &input_list)) {
        return NULL;
    }
    
    N = PyList_Size(input_list);
    d = PyList_Size(PyList_GetItem(input_list, 0));
    
    /* Convert Python list to C array */
    data = pylist_to_carray(input_list, N, d);
    if (data == NULL) return NULL;
    
    /* Compute norm matrix */
    A = sym(data, N, d);
    D = ddg(N, A);
    W = norm(N, D, A);
    
    /* Convert C array back to Python list */
    result = carray_to_pylist(W, N, N);
    
    /* Free memory */
    free_matrix_memory(W, N);
    free_matrix_memory(D, N);
    free_matrix_memory(A, N);
    free_matrix_memory(data, N);
    
    return result;
}

static PyObject* py_symnmf(PyObject *self, PyObject *args) {
    (void)self;  /* Prevent unused parameter warning */
    PyObject *norm_list, *initial_h_list;
    int N, k;
    double **W, **H;
    PyObject *result;

    /* Parse Python arguments */
    if (!PyArg_ParseTuple(args, "O!O!i", &PyList_Type, &norm_list, &PyList_Type, &initial_h_list, &k)) {
        return NULL;
    }
    
    N = PyList_Size(norm_list);
    
    /* Convert Python list to C array */
    W = pylist_to_carray(norm_list, N, N);
    if (W == NULL) return NULL;
    H = pylist_to_carray(initial_h_list, N, k);
    if (H == NULL) return NULL;
    
    /* Compute H matrix */
    symnmf(H, W, N, k);
    
    /* Convert C array back to Python list */
    result = carray_to_pylist(H, N, k);
    
    /* Free memory */
    free_matrix_memory(W, N);
    free_matrix_memory(H, N);
    
    return result;
}

static PyMethodDef SymNMFMethods[] = {
    {"sym", py_sym, METH_VARARGS, "Compute similarity matrix"},
    {"ddg", py_ddg, METH_VARARGS, "Compute diagonal degree matrix"},
    {"norm", py_norm, METH_VARARGS, "Compute normalized similarity matrix"},
    {"symnmf", py_symnmf, METH_VARARGS, "Perform full SymNMF algorithm"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef symnmfmodule = {
    PyModuleDef_HEAD_INIT,
    "symnmf_ext",  
    NULL,      
    -1,
    SymNMFMethods,
    NULL,
    NULL,
    NULL, 
    NULL
};

PyMODINIT_FUNC PyInit_symnmf_ext(void) {
    PyObject *m;
    m = PyModule_Create(&symnmfmodule);
    if (!m) return NULL;
    return m;
}