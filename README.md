# 🧩 Symmetric Non-Negative Matrix Factorization (SymNMF) Project

This project implements **Symmetric Non-Negative Matrix Factorization (SymNMF)** — a matrix factorization technique used for **clustering based on similarity graphs**.  
It is built as a hybrid system combining **C for performance-critical matrix computations** and **Python for usability, analysis, and evaluation**.

The project extends the **KMeans++** assignment by comparing **SymNMF-based clustering** with **KMeans clustering** using **silhouette analysis**.

---

## 📁 File Overview

| File | Description |
|------|--------------|
| `symnmf.c` | Core C implementation of the SymNMF algorithm and matrix operations (similarity, degree, normalization). |
| `symnmf.h` | Header file declaring all SymNMF-related functions. |
| `symnmfmodule.c` | C extension module exposing the SymNMF functions to Python through the **Python C API**. |
| `setup.py` | Build configuration for compiling the C extension module (`symnmf_ext.so`). |
| `symnmf.py` | Python script to interface with the C module and execute SymNMF tasks (`sym`, `ddg`, `norm`, `symnmf`). |
| `kmeans.py` | Reused and adapted from HW1 — standalone Python implementation of the KMeans algorithm. |
| `analysis.py` | Final analysis script comparing **SymNMF** and **KMeans** clustering results using **silhouette scores**. |
| `Makefile` | Compiles and links the core C files into an executable. |

---

## ⚙️ Building the Project

The project build process includes **two steps**:  
1️⃣ compiling the C code into an executable, and  
2️⃣ building the Python extension module.

### **Step 1 — Compile the C Code**

Use the provided `Makefile` to compile all C source files (`symnmf.c`, `symnmfmodule.c`, etc.):  

```bash
make
```

This command compiles and links the C components, creating the core executable (`symnmf`) and preparing the object files used in the Python extension.

### **Step 2 — Build the Python Extension**

Once the C files are compiled, build the Python C extension (`symnmf_ext.so`) with:

```bash
python3 setup.py build_ext --inplace
```

This creates a shared object file:

```
symnmf_ext.so
```

which enables Python to directly call C-implemented functions through the `symnmf_ext` module.

---

## 🚀 Running the Programs

### 1️⃣ Run the SymNMF Python Interface

```bash
python3 symnmf.py k goal data.csv
```

**Arguments:**

- `k` — Number of clusters (1 < k ≤ n)  
- `goal` — One of:  
  - `sym`  → Similarity matrix  
  - `ddg`  → Diagonal degree matrix  
  - `norm` → Normalized similarity matrix  
  - `symnmf` → Full SymNMF factorization  
- `data.csv`/`data.txt` — Input data file, comma-separated (3 input samples provided)

**Example:**  
```bash
python3 symnmf.py 3 symnmf data.csv
```

**Output:**  
A matrix \( H \) (size \( n x k \)), printed with values formatted to 4 decimal places.

---

### 2️⃣ Compare Clustering Results (Main Analysis Script)

Run:  
```bash
python3 analysis.py k data.csv
```

**What it does:**  
- Computes the normalized similarity matrix \( W \)  
- Initializes random \( H \)  
- Runs **SymNMF** via the C extension  
- Runs **KMeans** on the same dataset  
- Converts both outputs into cluster labels  
- Evaluates and prints **silhouette scores**  

**Example Output:**  
```
nmf: 0.6221
kmeans: 0.5798
```

---

### 3️⃣ Optional: Direct C Execution

You can directly test the C executable:  
```bash
./symnmf sym data.csv
```

**Goals available:**  
- `sym`  → prints similarity matrix  
- `ddg`  → prints diagonal degree matrix  
- `norm` → prints normalized similarity matrix  

---

## 🧩 Key Features

- Full implementation of **SymNMF** in pure C with dynamic memory handling.  
- Python–C integration using **Python C API** and `setuptools`.  
- Reuse of **KMeans** module from previous assignment for performance comparison.  
- Robust **error handling** and validation.  
- Evaluation via **silhouette metric** from `scikit-learn`.  

---

## 🧪 Requirements

- Python ≥ 3.6  
- NumPy  
- scikit-learn  
- GCC compiler  

Install Python dependencies:  
```bash
pip install numpy scikit-learn
```

