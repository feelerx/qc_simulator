import numpy as np
import enum
import time
import math

class GateType(enum.Enum):
    """Enumeration of quantum gate types."""
    GATE_X = 1  # Pauli-X gate
    GATE_H = 2  # Hadamard gate
    GATE_T = 3  # T gate

class Statevector:
    """Representation of a quantum statevector."""
    def __init__(self, num_qubits):
        """
        Initialize a statevector for given number of qubits.
        
        Args:
            num_qubits (int): Number of qubits in the system
        """
        self.num_qubits = num_qubits
        self.dimension = 1 << num_qubits
        self.data = np.zeros(self.dimension, dtype=np.float64)
        self.data[0] = 1.0  # Initialize to |0...0⟩

class Matrix:
    """Representation of a matrix."""
    def __init__(self, dimension):
        """
        Initialize a matrix with given dimension.
        
        Args:
            dimension (int): Dimension of the square matrix
        """
        self.dimension = dimension
        self.data = np.zeros(dimension * dimension, dtype=np.float64)

def create_statevector(num_qubits):
    """
    Create a statevector for given number of qubits.
    
    Args:
        num_qubits (int): Number of qubits
    
    Returns:
        Statevector: Initialized quantum statevector
    """
    return Statevector(num_qubits)

def create_matrix(dimension):
    """
    Create a matrix with given dimension.
    
    Args:
        dimension (int): Dimension of the matrix
    
    Returns:
        Matrix: Initialized matrix
    """
    return Matrix(dimension)

def create_single_qubit_gate(gate_type):
    """
    Create a single qubit gate matrix.
    
    Args:
        gate_type (GateType): Type of quantum gate
    
    Returns:
        Matrix: Gate matrix
    """
    gate = create_matrix(2)
    
    if gate_type == GateType.GATE_X:
        gate.data[0] = 0.0
        gate.data[1] = 1.0
        gate.data[2] = 1.0
        gate.data[3] = 0.0
    elif gate_type == GateType.GATE_H:
        sqrt_2_inv = 1.0 / math.sqrt(2)
        gate.data[0] = sqrt_2_inv
        gate.data[1] = sqrt_2_inv
        gate.data[2] = sqrt_2_inv
        gate.data[3] = -sqrt_2_inv
    elif gate_type == GateType.GATE_T:
        gate.data[0] = 1.0
        gate.data[1] = 0.0
        gate.data[2] = 0.0
        gate.data[3] = math.sqrt(2) / 2
    
    return gate

def apply_single_qubit_gate(sv, gate_type, target_qubit):
    """
    Apply a single qubit gate to a statevector.
    
    Args:
        sv (Statevector): Input statevector
        gate_type (GateType): Type of quantum gate
        target_qubit (int): Qubit to apply gate on
    """
    single_gate = create_single_qubit_gate(gate_type)
    full_gate = create_matrix(sv.dimension)
    
    # Create full gate matrix
    for i in range(sv.dimension):
        for j in range(sv.dimension):
            i_target = (i >> target_qubit) & 1
            j_target = (j >> target_qubit) & 1
            if (i & ~(1 << target_qubit)) == (j & ~(1 << target_qubit)):
                full_gate.data[i * sv.dimension + j] = single_gate.data[i_target * 2 + j_target]
    
    apply_matrix_to_statevector(sv, full_gate)

def apply_cnot(sv, control_qubit, target_qubit):
    """
    Apply CNOT gate to a statevector.
    
    Args:
        sv (Statevector): Input statevector
        control_qubit (int): Control qubit
        target_qubit (int): Target qubit
    """
    cnot = create_matrix(sv.dimension)
    
    # Initialize as identity matrix
    for i in range(sv.dimension):
        cnot.data[i * sv.dimension + i] = 1.0
    
    # Modify for CNOT operation
    for i in range(sv.dimension):
        control_bit = (i >> control_qubit) & 1
        if control_bit == 1:
            j = i ^ (1 << target_qubit)
            cnot.data[i * sv.dimension + i] = 0.0
            cnot.data[i * sv.dimension + j] = 1.0
    
    apply_matrix_to_statevector(sv, cnot)

def apply_matrix_to_statevector(sv, matrix):
    """
    Apply a matrix transformation to a statevector.
    
    Args:
        sv (Statevector): Input/output statevector
        matrix (Matrix): Transformation matrix
    """
    result = np.zeros(sv.dimension, dtype=np.float64)
    
    for i in range(sv.dimension):
        for j in range(sv.dimension):
            result[i] += matrix.data[i * sv.dimension + j] * sv.data[j]
    
    sv.data = result

def print_statevector(sv):
    """
    Print non-zero amplitudes of a statevector.
    
    Args:
        sv (Statevector): Statevector to print
    """
    print(f"Statevector ({sv.num_qubits} qubits):")
    for i in range(sv.dimension):
        if abs(sv.data[i]) > 1e-10:  # Only print non-zero amplitudes
            print(f"|{i}⟩: {sv.data[i]:.6f}")
    print()

def measure_runtime(num_qubits):
    """
    Measure runtime for a quantum circuit on given number of qubits.
    
    Args:
        num_qubits (int): Number of qubits
    
    Returns:
        float: Runtime in seconds
    """
    start_time = time.time()
    
    sv = create_statevector(num_qubits)
    
    # Apply test circuit
    apply_single_qubit_gate(sv, GateType.GATE_X, 0)
    apply_single_qubit_gate(sv, GateType.GATE_H, 1)
    if num_qubits > 1:
        apply_cnot(sv, 0, 1)
    
    end_time = time.time()
    return end_time - start_time

def save_runtime_data(num_qubits, time_taken):
    """
    Save runtime data to a file.
    
    Args:
        num_qubits (int): Number of qubits
        time_taken (float): Runtime in seconds
    """
    with open("runtime_data.txt", "a") as file:
        file.write(f"{num_qubits} {time_taken:.6f}\n")

def run_circuit_test(num_qubits):
    """
    Run a quantum circuit test for given number of qubits.
    
    Args:
        num_qubits (int): Number of qubits
    """
    sv = create_statevector(num_qubits)
    
    print(f"\nRunning {num_qubits} qubit circuit test:")
    print_statevector(sv)
    
    # Apply test circuit
    apply_single_qubit_gate(sv, GateType.GATE_X, num_qubits - 1)
    print(f"After X on qubit {num_qubits - 1}:")
    print_statevector(sv)
    
    apply_single_qubit_gate(sv, GateType.GATE_H, 0)
    print("After H on qubit 0:")
    print_statevector(sv)
    
    apply_cnot(sv, 0, num_qubits - 1)
    print(f"After CNOT (control: 0, target: {num_qubits - 1}):")
    print_statevector(sv)

def main():
    # Run circuit tests for 2-4 qubits
    for num_qubits in range(2, 5):
        run_circuit_test(num_qubits)
    
    # Measure runtime for increasing number of qubits
    for num_qubits in range(1, 15):
        time_taken = measure_runtime(num_qubits)
        save_runtime_data(num_qubits, time_taken)

if __name__ == "__main__":
    main()