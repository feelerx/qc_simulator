// quantum_simulator.h
#ifndef QUANTUM_SIMULATOR_H
#define QUANTUM_SIMULATOR_H

#include <stdbool.h>

#define MAX_QUBITS 29
#define RUNTIME_DATA_FILE "runtime_data.txt"

typedef struct {
    double elements[2][2];
} QuantumGate;

// Gate operations
bool apply_single_qubit_gate(double* state_vector, QuantumGate gate, int num_qubits, int target_qubit);
bool apply_cnot_gate(double* state_vector, int num_qubits, int control_qubit, int target_qubit);
bool tensor_contract(double* state_vector, QuantumGate gate, int target_qubit, int num_qubits);

// State management
bool initialize_state_vector(double* state_vector, int num_qubits);
void print_state_vector(const double* state_vector, int num_qubits);

// Testing and benchmarking
void run_quantum_circuit_test(int num_qubits);
double measure_circuit_runtime(int num_qubits);
void save_runtime_data(int num_qubits, double time_taken);

// Validation
bool validate_qubit_indices(int num_qubits, int qubit1, int qubit2);
bool validate_single_qubit(int num_qubits, int qubit);

#endif // QUANTUM_SIMULATOR_H
