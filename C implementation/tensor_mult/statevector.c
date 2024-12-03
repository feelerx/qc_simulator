// quantum_simulator.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "quantum_simulator.h"

// Standard gates
QuantumGate get_hadamard_gate(void) {
    double inv_sqrt_2 = 1.0 / sqrt(2);
    QuantumGate H = {
        .elements = {
            {inv_sqrt_2,  inv_sqrt_2},
            {inv_sqrt_2, -inv_sqrt_2}
        }
    };
    return H;
}

QuantumGate get_x_gate(void) {
    QuantumGate X = {
        .elements = {
            {0.0, 1.0},
            {1.0, 0.0}
        }
    };
    return X;
}

bool validate_single_qubit(int num_qubits, int qubit) {
    return qubit >= 0 && qubit < num_qubits;
}

bool validate_qubit_indices(int num_qubits, int qubit1, int qubit2) {
    if (!validate_single_qubit(num_qubits, qubit1) || 
        !validate_single_qubit(num_qubits, qubit2)) {
        fprintf(stderr, "Error: Invalid qubit indices\n");
        return false;
    }
    if (qubit1 == qubit2) {
        fprintf(stderr, "Error: Control and target qubits must be different\n");
        return false;
    }
    return true;
}

bool initialize_state_vector(double* state_vector, int num_qubits) {
    if (!state_vector) return false;
    
    int dim = 1 << num_qubits;
    memset(state_vector, 0, dim * sizeof(double));
    state_vector[0] = 1.0;  // Initialize to |0⟩⊗n state
    return true;
}

void print_state_vector(const double* state_vector, int num_qubits) {
    if (!state_vector) return;
    
    int dim = 1 << num_qubits;
    for (int i = 0; i < dim; i++) {
        printf("|%d⟩: %.6f\n", i, state_vector[i]);
    }
    printf("\n");
}

bool tensor_contract(double* state_vector, QuantumGate gate, int target_qubit, int num_qubits) {
    int dim = 1 << num_qubits;
    double* new_state = calloc(dim, sizeof(double));
    if (!new_state) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return false;
    }

    for (int i = 0; i < dim; i++) {
        int bit_mask = 1 << target_qubit;
        int basis_0 = i & ~bit_mask;
        int basis_1 = i | bit_mask;
        
        if ((i & bit_mask) == 0) {
            new_state[basis_0] += gate.elements[0][0] * state_vector[i];
            new_state[basis_1] += gate.elements[1][0] * state_vector[i];
        } else {
            new_state[basis_0] += gate.elements[0][1] * state_vector[i];
            new_state[basis_1] += gate.elements[1][1] * state_vector[i];
        }
    }

    memcpy(state_vector, new_state, dim * sizeof(double));
    free(new_state);
    return true;
}

bool apply_single_qubit_gate(double* state_vector, QuantumGate gate, int num_qubits, int target_qubit) {
    if (!state_vector || !validate_single_qubit(num_qubits, target_qubit)) {
        fprintf(stderr, "Error: Invalid parameters for single qubit gate\n");
        return false;
    }
    return tensor_contract(state_vector, gate, target_qubit, num_qubits);
}

bool apply_cnot_gate(double* state_vector, int num_qubits, int control_qubit, int target_qubit) {
    if (!state_vector || !validate_qubit_indices(num_qubits, control_qubit, target_qubit)) {
        return false;
    }

    int dim = 1 << num_qubits;
    double* new_state = calloc(dim, sizeof(double));
    if (!new_state) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return false;
    }

    for (int i = 0; i < dim; i++) {
        int control_bit = (i >> control_qubit) & 1;
        if (control_bit == 1) {
            int new_state_idx = i ^ (1 << target_qubit);
            new_state[i] = state_vector[new_state_idx];
        } else {
            new_state[i] = state_vector[i];
        }
    }

    memcpy(state_vector, new_state, dim * sizeof(double));
    free(new_state);
    return true;
}

void run_quantum_circuit_test(int num_qubits) {
    int dim = 1 << num_qubits;
    double* state_vector = calloc(dim, sizeof(double));
    if (!state_vector || !initialize_state_vector(state_vector, num_qubits)) {
        fprintf(stderr, "Error: Failed to initialize quantum state\n");
        free(state_vector);
        return;
    }

    QuantumGate H = get_hadamard_gate();
    QuantumGate X = get_x_gate();

    printf("Testing %d qubit circuit:\n", num_qubits);
    printf("Initial state:\n");
    print_state_vector(state_vector, num_qubits);

    // Apply test circuit
    if (!apply_single_qubit_gate(state_vector, X, num_qubits, num_qubits - 1)) {
        fprintf(stderr, "Error applying X gate\n");
        free(state_vector);
        return;
    }
    printf("After X on qubit %d:\n", num_qubits - 1);
    print_state_vector(state_vector, num_qubits);

    if (!apply_single_qubit_gate(state_vector, H, num_qubits, 0)) {
        fprintf(stderr, "Error applying H gate\n");
        free(state_vector);
        return;
    }
    printf("After H on qubit 0:\n");
    print_state_vector(state_vector, num_qubits);

    if (!apply_cnot_gate(state_vector, num_qubits, 0, num_qubits - 1)) {
        fprintf(stderr, "Error applying CNOT gate\n");
        free(state_vector);
        return;
    }
    printf("After CNOT (control: 0, target: %d):\n", num_qubits - 1);
    print_state_vector(state_vector, num_qubits);

    free(state_vector);
}

double measure_circuit_runtime(int num_qubits) {
    int dim = 1 << num_qubits;
    double* state_vector = calloc(dim, sizeof(double));
    if (!state_vector || !initialize_state_vector(state_vector, num_qubits)) {
        fprintf(stderr, "Error: Failed to initialize quantum state\n");
        free(state_vector);
        return -1.0;
    }

    QuantumGate H = get_hadamard_gate();
    QuantumGate X = get_x_gate();

    clock_t start_time = clock();
    
    // Standard test circuit
    apply_single_qubit_gate(state_vector, H, num_qubits, 0);
    apply_single_qubit_gate(state_vector, X, num_qubits, 1);
    apply_cnot_gate(state_vector, num_qubits, 0, 1);
    
    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    free(state_vector);
    return time_taken;
}

void save_runtime_data(int num_qubits, double time_taken) {
    FILE *file = fopen(RUNTIME_DATA_FILE, "a");
    if (!file) {
        fprintf(stderr, "Error: Could not open runtime data file\n");
        return;
    }
    fprintf(file, "%d %.6f\n", num_qubits, time_taken);
    fclose(file);
}

// main.c
int main(void) {
    // Run standard tests for small number of qubits
    for (int num_qubits = 2; num_qubits <= 4; num_qubits++) {
        run_quantum_circuit_test(num_qubits);
    }

    // Run performance tests
    printf("\nRunning performance tests...\n");
    for (int num_qubits = 2; num_qubits <= MAX_QUBITS; num_qubits++) {
        double runtime = measure_circuit_runtime(num_qubits);
        if (runtime >= 0) {
            save_runtime_data(num_qubits, runtime);
            printf("Completed test for %d qubits: %.6f seconds\n", num_qubits, runtime);
        }
    }

    return 0;
}