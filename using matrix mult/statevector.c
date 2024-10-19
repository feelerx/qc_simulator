#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void kronecker(double* A, int rowsA, int colsA, double* B, int rowsB, int colsB, double* result);
void apply_matrix(double *statevector, double *matrix, int size);
void create_single_qubit_gate_matrix(double *matrix, double *gate, int num_qubits, int target_qubit);
void create_cnot_matrix(double *matrix, int num_qubits, int control_qubit, int target_qubit);
void print_statevector(double *statevector, int num_qubits);
void save_runtime_data(int num_qubits, double time_taken);
double test_runtime(int num_qubit);
void run_test(int num_qubits);

int main(void) {
    int num_qubits = 3;  // Example with 3 qubits
    
    // Initialize statevector to |000⟩
    int dim = 1 << num_qubits;  // Size of statevector (2^num_qubits)
    double* statevector = (double*) malloc(dim * sizeof(double));
    statevector[0] = 1.0;  // Initialize to |000⟩
    for (int i = 1; i < dim; i++) {
        statevector[i] = 0.0;
    }

    // Print initial statevector
    print_statevector(statevector, num_qubits);

    // Define X and H matrices
    double X[4] = {0.0, 1.0, 1.0, 0.0};  // Pauli-X matrix
    double H[4] = {1.0/sqrt(2), 1.0/sqrt(2), 1.0/sqrt(2), -1.0/sqrt(2)};  // Hadamard matrix

    // Create and apply X gate on qubit 1 (target qubit)
    double* full_X_matrix = (double*) malloc(dim * dim * sizeof(double));
    create_single_qubit_gate_matrix(full_X_matrix, X, num_qubits, 2);
    apply_matrix(statevector, full_X_matrix, dim);
    print_statevector(statevector, num_qubits);

    // Create and apply H gate on qubit 0 (target qubit)
    double* full_H_matrix = (double*) malloc(dim * dim * sizeof(double));
    create_single_qubit_gate_matrix(full_H_matrix, H, num_qubits, 1);
    apply_matrix(statevector, full_H_matrix, dim);
    print_statevector(statevector, num_qubits);

    // Define and create the CNOT matrix (control = qubit 0, target = qubit 1)
    double* cnot_matrix = (double*) malloc(dim * dim * sizeof(double));
    create_cnot_matrix(cnot_matrix, num_qubits, 0, 2);
    // Apply the CNOT matrix to the statevector
    apply_matrix(statevector, cnot_matrix, dim);
    print_statevector(statevector, num_qubits);


    //TESTING AREA

    int max_qubits = 15;  // Maximum number of qubits to simulate

    for (int num_qubit = 1; num_qubit <= max_qubits; num_qubit++) {
        double time_taken = test_runtime(num_qubit);  // Test runtime for current number of qubits
        save_runtime_data(num_qubit, time_taken);     // Save the result to file
    }

    for (int num_qubits = 2; num_qubits <= 4; num_qubits++) {
        run_test(num_qubits);
    }


    free(statevector);
    free(full_X_matrix);
    free(full_H_matrix);
    free(cnot_matrix);

    return 0;
}

// Function to calculate Kronecker product of two matrices
void kronecker(double* A, int rowsA, int colsA, double* B, int rowsB, int colsB, double* result) {
    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsA; j++) {
            for (int k = 0; k < rowsB; k++) {
                for (int l = 0; l < colsB; l++) {
                    result[(i * rowsB + k) * (colsA * colsB) + (j * colsB + l)] = A[i * colsA + j] * B[k * colsB + l];
                }
            }
        }
    }
}

// General function to apply a gate to the statevector using matrix multiplication
void apply_matrix(double *statevector, double *matrix, int size) {
    double *result = (double*) malloc(size * sizeof(double));

    for (int i = 0; i < size; i++) {
        result[i] = 0;
        for (int j = 0; j < size; j++) {
            result[i] += matrix[i * size + j] * statevector[j];
        }
    }

    // Copy result back to statevector
    for (int i = 0; i < size; i++) {
        statevector[i] = result[i];
    }

    free(result);
}

void create_single_qubit_gate_matrix(double *matrix, double *gate, int num_qubits, int target_qubit) {
    int dim = 1 << num_qubits;  // 2^num_qubits
    int gate_dim = 2;           // Single qubit gate is 2x2

    // Initialize the matrix as identity
    for (int i = 0; i < dim * dim; i++) {
        matrix[i] = (i / dim == i % dim) ? 1.0 : 0.0;
    }

    // Apply the gate to the target qubit
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            int i_target = (i >> target_qubit) & 1;
            int j_target = (j >> target_qubit) & 1;
            if ((i & ~(1 << target_qubit)) == (j & ~(1 << target_qubit))) {
                matrix[i * dim + j] = gate[i_target * 2 + j_target];
            }
        }
    }
}

// Function to create the CNOT matrix for arbitrary control and target qubits
void create_cnot_matrix(double *matrix, int num_qubits, int control_qubit, int target_qubit) {
    int dim = 1 << num_qubits;  // 2^num_qubits, size of the statevector

    // Initialize the CNOT matrix as identity
    for (int i = 0; i < dim * dim; i++) {
        matrix[i] = 0.0;
    }
    for (int i = 0; i < dim; i++) {
        matrix[i * dim + i] = 1.0;  // Identity matrix
    }

    // Modify the matrix for the control and target qubits
    for (int i = 0; i < dim; i++) {
        int control_bit = (i >> control_qubit) & 1;
        int target_bit = (i >> target_qubit) & 1;

        // If control qubit is 1, swap the target bit
        if (control_bit == 1) {
            int j = i ^ (1 << target_qubit);  // Flip the target qubit
            matrix[i * dim + i] = 0.0;  // Zero out the diagonal entry
            matrix[i * dim + j] = 1.0;  // Swap the off-diagonal entry
        }
    }
}

// Function to print the statevector
void print_statevector(double *statevector, int num_qubits) {
    int dim = 1 << num_qubits;  // 2^num_qubits
    for (int i = 0; i < dim; i++) {
        printf("state[%d] = %f\n", i, statevector[i]);
    }
    printf("\n");
}

void save_runtime_data(int num_qubits, double time_taken) {
    FILE *file = fopen("runtime_data.txt", "a");  // Open the file in append mode
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }
    fprintf(file, "%d %f\n", num_qubits, time_taken);  // Write num_qubits and time_taken
    fclose(file);
}

// Function to test and return the runtime for a given number of qubits
double test_runtime(int num_qubit) {
    int dim = 1 << num_qubit;  // Dimension of the statevector (2^num_qubit)
    double* statevector = (double*) malloc(dim * sizeof(double));  // Allocate memory for statevector

    for (int i = 0; i < dim; i++) {
        statevector[i] = (i == 0) ? 1.0 : 0.0;  // Initialize to |0...0⟩
    }

    // Start measuring time
    clock_t start_time = clock();

    // Apply gates (X, H, CNOT) on various qubits
    // Define X and H matrices
    double X[4] = {0.0, 1.0, 1.0, 0.0};  // Pauli-X matrix
    double H[4] = {1.0/sqrt(2), 1.0/sqrt(2), 1.0/sqrt(2), -1.0/sqrt(2)};  // Hadamard matrix

    // Create and apply X gate on qubit 1 (target qubit)
    double* full_X_matrix = (double*) malloc(dim * dim * sizeof(double));
    create_single_qubit_gate_matrix(full_X_matrix, X, num_qubit, 2);
    apply_matrix(statevector, full_X_matrix, dim);

    // Create and apply H gate on qubit 0 (target qubit)
    double* full_H_matrix = (double*) malloc(dim * dim * sizeof(double));
    create_single_qubit_gate_matrix(full_H_matrix, H, num_qubit, 1);
    apply_matrix(statevector, full_H_matrix, dim);

    if (num_qubit > 1) {
        double* cnot_matrix = (double*) malloc(dim * dim * sizeof(double));
        create_cnot_matrix(cnot_matrix, num_qubit, 0, 2);
        // Apply the CNOT matrix to the statevector
        apply_matrix(statevector, cnot_matrix, dim);  // Only if there are at least 2 qubits
        free(cnot_matrix);
    }

    // Stop measuring time
    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Free memory
    free(statevector);
    free(full_X_matrix);
    free(full_H_matrix);


    // Return the time taken
    return time_taken;
}

void run_test(int num_qubits) {
    int dim = 1 << num_qubits;
    double* statevector = (double*) malloc(dim * sizeof(double));
    
    // Initialize state to |0...0>
    statevector[0] = 1.0;
    for (int i = 1; i < dim; i++) {
        statevector[i] = 0.0;
    }

    printf("Testing %d qubit circuit:\n", num_qubits);
    printf("Initial state:\n");
    print_statevector(statevector, num_qubits);

    // Define gates
    double X[4] = {0.0, 1.0, 1.0, 0.0};  // Pauli-X
    double H[4] = {1.0/sqrt(2), 1.0/sqrt(2), 1.0/sqrt(2), -1.0/sqrt(2)};  // Hadamard

    // Apply X to last qubit
    double* full_X_matrix = (double*) malloc(dim * dim * sizeof(double));
    create_single_qubit_gate_matrix(full_X_matrix, X, num_qubits, num_qubits - 1);
    apply_matrix(statevector, full_X_matrix, dim);
    printf("After X on qubit %d:\n", num_qubits - 1);
    print_statevector(statevector, num_qubits);

    // Apply H to first qubit
    double* full_H_matrix = (double*) malloc(dim * dim * sizeof(double));
    create_single_qubit_gate_matrix(full_H_matrix, H, num_qubits, 0);
    apply_matrix(statevector, full_H_matrix, dim);
    printf("After H on qubit 0:\n");
    print_statevector(statevector, num_qubits);

    // Apply CNOT with control on first qubit and target on last qubit
    double* cnot_matrix = (double*) malloc(dim * dim * sizeof(double));
    create_cnot_matrix(cnot_matrix, num_qubits, 0, num_qubits - 1);
    apply_matrix(statevector, cnot_matrix, dim);
    printf("After CNOT (control: 0, target: %d):\n", num_qubits - 1);
    print_statevector(statevector, num_qubits);

    // Free memory
    free(statevector);
    free(full_X_matrix);
    free(full_H_matrix);
    free(cnot_matrix);

    printf("\n");
}
