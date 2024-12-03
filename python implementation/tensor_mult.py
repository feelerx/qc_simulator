import numpy as np
import time
from dataclasses import dataclass
from typing import List, Tuple

# Constants
MAX_QUBITS = 25
RUNTIME_DATA_FILE = "runtime_data.txt"

@dataclass
class QuantumGate:
    elements: np.ndarray

    def __init__(self, elements: List[List[float]]):
        self.elements = np.array(elements)

class QuantumSimulator:
    @staticmethod
    def get_hadamard_gate() -> QuantumGate:
        """Returns a Hadamard gate."""
        inv_sqrt_2 = 1.0 / np.sqrt(2)
        return QuantumGate([
            [inv_sqrt_2,  inv_sqrt_2],
            [inv_sqrt_2, -inv_sqrt_2]
        ])

    @staticmethod
    def get_x_gate() -> QuantumGate:
        """Returns a Pauli-X (NOT) gate."""
        return QuantumGate([
            [0.0, 1.0],
            [1.0, 0.0]
        ])

    @staticmethod
    def validate_single_qubit(num_qubits: int, qubit: int) -> bool:
        """Validates if a qubit index is valid."""
        return 0 <= qubit < num_qubits

    @staticmethod
    def validate_qubit_indices(num_qubits: int, qubit1: int, qubit2: int) -> bool:
        """Validates if two qubit indices are valid and different."""
        if not (QuantumSimulator.validate_single_qubit(num_qubits, qubit1) and 
                QuantumSimulator.validate_single_qubit(num_qubits, qubit2)):
            print("Error: Invalid qubit indices")
            return False
        if qubit1 == qubit2:
            print("Error: Control and target qubits must be different")
            return False
        return True

    @staticmethod
    def initialize_state_vector(num_qubits: int) -> np.ndarray:
        """Initializes a quantum state vector to |0⟩⊗n state."""
        dim = 1 << num_qubits
        state_vector = np.zeros(dim)
        state_vector[0] = 1.0
        return state_vector

    @staticmethod
    def print_state_vector(state_vector: np.ndarray):
        """Prints the quantum state vector."""
        for i, amplitude in enumerate(state_vector):
            print(f"|{i}⟩: {amplitude:.6f}")
        print()

    @staticmethod
    def tensor_contract(state_vector: np.ndarray, gate: QuantumGate, 
                       target_qubit: int, num_qubits: int) -> np.ndarray:
        """Applies a quantum gate to a specific qubit in the state vector."""
        dim = 1 << num_qubits
        new_state = np.zeros(dim)

        for i in range(dim):
            bit_mask = 1 << target_qubit
            basis_0 = i & ~bit_mask
            basis_1 = i | bit_mask

            if (i & bit_mask) == 0:
                new_state[basis_0] += gate.elements[0][0] * state_vector[i]
                new_state[basis_1] += gate.elements[1][0] * state_vector[i]
            else:
                new_state[basis_0] += gate.elements[0][1] * state_vector[i]
                new_state[basis_1] += gate.elements[1][1] * state_vector[i]

        return new_state

    @staticmethod
    def apply_single_qubit_gate(state_vector: np.ndarray, gate: QuantumGate, 
                               num_qubits: int, target_qubit: int) -> Tuple[bool, np.ndarray]:
        """Applies a single-qubit gate to the quantum state."""
        if not QuantumSimulator.validate_single_qubit(num_qubits, target_qubit):
            print("Error: Invalid parameters for single qubit gate")
            return False, state_vector
        return True, QuantumSimulator.tensor_contract(state_vector, gate, target_qubit, num_qubits)

    @staticmethod
    def apply_cnot_gate(state_vector: np.ndarray, num_qubits: int, 
                        control_qubit: int, target_qubit: int) -> Tuple[bool, np.ndarray]:
        """Applies a CNOT gate to the quantum state."""
        if not QuantumSimulator.validate_qubit_indices(num_qubits, control_qubit, target_qubit):
            return False, state_vector

        dim = 1 << num_qubits
        new_state = np.zeros(dim)

        for i in range(dim):
            control_bit = (i >> control_qubit) & 1
            if control_bit == 1:
                new_state_idx = i ^ (1 << target_qubit)
                new_state[i] = state_vector[new_state_idx]
            else:
                new_state[i] = state_vector[i]

        return True, new_state

    @staticmethod
    def run_quantum_circuit_test(num_qubits: int):
        """Runs a test quantum circuit with specified number of qubits."""
        state_vector = QuantumSimulator.initialize_state_vector(num_qubits)
        H = QuantumSimulator.get_hadamard_gate()
        X = QuantumSimulator.get_x_gate()

        print(f"Testing {num_qubits} qubit circuit:")
        print("Initial state:")
        QuantumSimulator.print_state_vector(state_vector)

        # Apply test circuit
        success, state_vector = QuantumSimulator.apply_single_qubit_gate(
            state_vector, X, num_qubits, num_qubits - 1)
        if not success:
            print("Error applying X gate")
            return
        print(f"After X on qubit {num_qubits - 1}:")
        QuantumSimulator.print_state_vector(state_vector)

        success, state_vector = QuantumSimulator.apply_single_qubit_gate(
            state_vector, H, num_qubits, 0)
        if not success:
            print("Error applying H gate")
            return
        print("After H on qubit 0:")
        QuantumSimulator.print_state_vector(state_vector)

        success, state_vector = QuantumSimulator.apply_cnot_gate(
            state_vector, num_qubits, 0, num_qubits - 1)
        if not success:
            print("Error applying CNOT gate")
            return
        print(f"After CNOT (control: 0, target: {num_qubits - 1}):")
        QuantumSimulator.print_state_vector(state_vector)

    @staticmethod
    def measure_circuit_runtime(num_qubits: int) -> float:
        """Measures the runtime of a standard test circuit."""
        state_vector = QuantumSimulator.initialize_state_vector(num_qubits)
        H = QuantumSimulator.get_hadamard_gate()
        X = QuantumSimulator.get_x_gate()

        start_time = time.time()
        
        # Standard test circuit
        success, state_vector = QuantumSimulator.apply_single_qubit_gate(state_vector, H, num_qubits, 0)
        success, state_vector = QuantumSimulator.apply_single_qubit_gate(state_vector, X, num_qubits, 1)
        success, state_vector = QuantumSimulator.apply_cnot_gate(state_vector, num_qubits, 0, 1)
        
        end_time = time.time()
        return end_time - start_time

    @staticmethod
    def save_runtime_data(num_qubits: int, time_taken: float):
        """Saves runtime data to a file."""
        try:
            with open(RUNTIME_DATA_FILE, "a") as file:
                file.write(f"{num_qubits} {time_taken:.6f}\n")
        except IOError:
            print("Error: Could not open runtime data file")

def main():
    # Run standard tests for small number of qubits
    for num_qubits in range(2, 5):
        QuantumSimulator.run_quantum_circuit_test(num_qubits)

    # Run performance tests
    print("\nRunning performance tests...")
    for num_qubits in range(2, MAX_QUBITS + 1):
        runtime = QuantumSimulator.measure_circuit_runtime(num_qubits)
        if runtime >= 0:
            QuantumSimulator.save_runtime_data(num_qubits, runtime)
            print(f"Completed test for {num_qubits} qubits: {runtime:.6f} seconds")

if __name__ == "__main__":
    main()