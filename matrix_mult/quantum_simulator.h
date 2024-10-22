// quantum_simulator.h
#ifndef QUANTUM_SIMULATOR_H
#define QUANTUM_SIMULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Data structures
typedef struct {
    double* data;
    int num_qubits;
    int dimension;
} Statevector;

typedef struct {
    double* data;
    int dimension;
} Matrix;

// Common quantum gates
typedef enum {
    GATE_X,    // Pauli-X gate
    GATE_H,    // Hadamard gate
    GATE_T     // T gate
} GateType;

// Core functions
Statevector* create_statevector(int num_qubits);
void free_statevector(Statevector* sv);
Matrix* create_matrix(int dimension);
void free_matrix(Matrix* matrix);

// Gate operations
Matrix* create_single_qubit_gate(GateType gate_type);
void apply_single_qubit_gate(Statevector* sv, GateType gate_type, int target_qubit);
void apply_cnot(Statevector* sv, int control_qubit, int target_qubit);
void apply_matrix_to_statevector(Statevector* sv, Matrix* matrix);

// Utility functions
void print_statevector(const Statevector* sv);
double measure_runtime(int num_qubits);
void save_runtime_data(int num_qubits, double time_taken);
void run_circuit_test(int num_qubits);

#endif // QUANTUM_SIMULATOR_H