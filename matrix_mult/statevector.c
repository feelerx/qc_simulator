// quantum_simulator.c
#include "quantum_simulator.h"

// Implementation of core functions
Statevector* create_statevector(int num_qubits) {
    Statevector* sv = malloc(sizeof(Statevector));
    sv->num_qubits = num_qubits;
    sv->dimension = 1 << num_qubits;
    sv->data = calloc(sv->dimension, sizeof(double));
    sv->data[0] = 1.0;  // Initialize to |0...0⟩
    return sv;
}

void free_statevector(Statevector* sv) {
    free(sv->data);
    free(sv);
}

Matrix* create_matrix(int dimension) {
    Matrix* matrix = malloc(sizeof(Matrix));
    matrix->dimension = dimension;
    matrix->data = calloc(dimension * dimension, sizeof(double));
    return matrix;
}

void free_matrix(Matrix* matrix) {
    free(matrix->data);
    free(matrix);
}

// Gate creation functions
Matrix* create_single_qubit_gate(GateType gate_type) {
    Matrix* gate = create_matrix(2);
    
    switch (gate_type) {
        case GATE_X:
            gate->data[0] = 0.0; gate->data[1] = 1.0;
            gate->data[2] = 1.0; gate->data[3] = 0.0;
            break;
        case GATE_H:
            gate->data[0] = 1.0/sqrt(2); gate->data[1] = 1.0/sqrt(2);
            gate->data[2] = 1.0/sqrt(2); gate->data[3] = -1.0/sqrt(2);
            break;
        case GATE_T:
            gate->data[0] = 1.0; gate->data[1] = 0.0;
            gate->data[2] = 0.0; gate->data[3] = sqrt(2)/2;
            break;
    }
    
    return gate;
}

void apply_single_qubit_gate(Statevector* sv, GateType gate_type, int target_qubit) {
    Matrix* single_gate = create_single_qubit_gate(gate_type);
    Matrix* full_gate = create_matrix(sv->dimension);
    
    // Create full gate matrix
    for (int i = 0; i < sv->dimension; i++) {
        for (int j = 0; j < sv->dimension; j++) {
            int i_target = (i >> target_qubit) & 1;
            int j_target = (j >> target_qubit) & 1;
            if ((i & ~(1 << target_qubit)) == (j & ~(1 << target_qubit))) {
                full_gate->data[i * sv->dimension + j] = single_gate->data[i_target * 2 + j_target];
            }
        }
    }
    
    apply_matrix_to_statevector(sv, full_gate);
    
    free_matrix(single_gate);
    free_matrix(full_gate);
}

void apply_cnot(Statevector* sv, int control_qubit, int target_qubit) {
    Matrix* cnot = create_matrix(sv->dimension);
    
    // Initialize as identity matrix
    for (int i = 0; i < sv->dimension; i++) {
        cnot->data[i * sv->dimension + i] = 1.0;
    }
    
    // Modify for CNOT operation
    for (int i = 0; i < sv->dimension; i++) {
        int control_bit = (i >> control_qubit) & 1;
        if (control_bit == 1) {
            int j = i ^ (1 << target_qubit);
            cnot->data[i * sv->dimension + i] = 0.0;
            cnot->data[i * sv->dimension + j] = 1.0;
        }
    }
    
    apply_matrix_to_statevector(sv, cnot);
    free_matrix(cnot);
}

void apply_matrix_to_statevector(Statevector* sv, Matrix* matrix) {
    double* result = calloc(sv->dimension, sizeof(double));
    
    for (int i = 0; i < sv->dimension; i++) {
        for (int j = 0; j < sv->dimension; j++) {
            result[i] += matrix->data[i * sv->dimension + j] * sv->data[j];
        }
    }
    
    // Copy result back to statevector
    for (int i = 0; i < sv->dimension; i++) {
        sv->data[i] = result[i];
    }
    
    free(result);
}

// Utility functions
void print_statevector(const Statevector* sv) {
    printf("Statevector (%d qubits):\n", sv->num_qubits);
    for (int i = 0; i < sv->dimension; i++) {
        if (fabs(sv->data[i]) > 1e-10) {  // Only print non-zero amplitudes
            printf("|%d⟩: %.6f\n", i, sv->data[i]);
        }
    }
    printf("\n");
}

double measure_runtime(int num_qubits) {
    clock_t start = clock();
    
    Statevector* sv = create_statevector(num_qubits);
    
    // Apply test circuit
    apply_single_qubit_gate(sv, GATE_X, 0);
    apply_single_qubit_gate(sv, GATE_H, 1);
    if (num_qubits > 1) {
        apply_cnot(sv, 0, 1);
    }
    
    clock_t end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    
    free_statevector(sv);
    return time_taken;
}

void save_runtime_data(int num_qubits, double time_taken) {
    FILE* file = fopen("runtime_data.txt", "a");
    if (file == NULL) {
        fprintf(stderr, "Error opening runtime_data.txt\n");
        return;
    }
    fprintf(file, "%d %.6f\n", num_qubits, time_taken);
    fclose(file);
}

void run_circuit_test(int num_qubits) {
    Statevector* sv = create_statevector(num_qubits);
    
    printf("\nRunning %d qubit circuit test:\n", num_qubits);
    print_statevector(sv);
    
    // Apply test circuit
    apply_single_qubit_gate(sv, GATE_X, num_qubits - 1);
    printf("After X on qubit %d:\n", num_qubits - 1);
    print_statevector(sv);
    
    apply_single_qubit_gate(sv, GATE_H, 0);
    printf("After H on qubit 0:\n");
    print_statevector(sv);
    
    apply_cnot(sv, 0, num_qubits - 1);
    printf("After CNOT (control: 0, target: %d):\n", num_qubits - 1);
    print_statevector(sv);
    
    free_statevector(sv);
}

// main.c
int main(void) {
    // Run circuit tests for 2-4 qubits
    for (int num_qubits = 2; num_qubits <= 4; num_qubits++) {
        run_circuit_test(num_qubits);
    }
    
    // Measure runtime for increasing number of qubits
    for (int num_qubits = 1; num_qubits <= 14; num_qubits++) {
        double time_taken = measure_runtime(num_qubits);
        save_runtime_data(num_qubits, time_taken);
    }
    
    return 0;
}