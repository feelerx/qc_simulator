import numpy as np
import time

def apply_x(statevector, num_qubit, target):
    """Apply the X (Pauli-X) gate on a target qubit."""
    dim = 2 ** num_qubit
    for i in range(dim):
        # Check if the target qubit is 0, then swap with the state where target is 1
        if not (i & (1 << target)):
            swap_index = i | (1 << target)  # Set target qubit to 1
            statevector[i], statevector[swap_index] = statevector[swap_index], statevector[i]
    return statevector

def apply_h(statevector, num_qubit, target):
    """Apply the H (Hadamard) gate on a target qubit."""
    dim = 2 ** num_qubit
    sqrt_2_inv = 1.0 / np.sqrt(2.0)
    
    for i in range(dim):
        # Check if the target qubit is 0, then combine the states where target is 0 and 1
        if not (i & (1 << target)):
            target_1_index = i | (1 << target)  # Set target qubit to 1
            a = statevector[i]     # Amplitude where target qubit is 0
            b = statevector[target_1_index]  # Amplitude where target qubit is 1

            # Update the amplitudes according to Hadamard transformation
            statevector[i] = sqrt_2_inv * (a + b)
            statevector[target_1_index] = sqrt_2_inv * (a - b)
    return statevector

def apply_cnot(statevector, num_qubit, control, target):
    """Apply the CNOT gate."""
    dim = 2 ** num_qubit
    for i in range(dim):
        # Check if control qubit is 1
        if i & (1 << control):
            # Get the indices for the target qubit being 0 or 1
            target0 = i & ~(1 << target)  # target = 0
            target1 = i | (1 << target)   # target = 1
            
            # Swap amplitudes for target qubit being 0 and 1
            statevector[target0], statevector[target1] = statevector[target1], statevector[target0]
    return statevector

def print_statevector(statevector, num_qubit):
    """Print the current statevector."""
    dim = 2 ** num_qubit
    for i in range(dim):
        print(f"state[{i}] = {statevector[i]}")
    print()

def initialize_statevector(num_qubit):
    """Initialize statevector to |0...0⟩ state."""
    dim = 2 ** num_qubit
    statevector = np.zeros(dim)
    statevector[0] = 1.0  # Set first state to 1
    return statevector

def save_runtime_data(num_qubits, time_taken):
    """Save runtime data to a file."""
    with open("runtime_data.txt", "a") as file:
        file.write(f"{num_qubits} {time_taken}\n")

def test_runtime(num_qubit):
    """Test and return runtime for a given number of qubits."""
    # Initialize statevector
    statevector = initialize_statevector(num_qubit)

    # Start measuring time
    start_time = time.time()

    # Apply gates (X, H, CNOT) on various qubits
    statevector = apply_x(statevector, num_qubit, 0)
    statevector = apply_h(statevector, num_qubit, 0)
    if num_qubit > 1:
        statevector = apply_cnot(statevector, num_qubit, 0, 1)  # Only if there are at least 2 qubits

    # Stop measuring time
    end_time = time.time()
    time_taken = end_time - start_time

    return time_taken

def main():
    # Example usage similar to the C code
    num_qubit = 4
    statevector = initialize_statevector(num_qubit)

    print("Initial State:")
    print_statevector(statevector, num_qubit)

    # Apply X gate on qubit 2
    statevector = apply_x(statevector, num_qubit, 2)
    print("After X gate on qubit 2:")
    print_statevector(statevector, num_qubit)

    # Apply H gate on qubit 1
    statevector = apply_h(statevector, num_qubit, 1)
    print("After H gate on qubit 1:")
    print_statevector(statevector, num_qubit)

    # Apply CNOT gate (control qubit 0, target qubit 2)
    statevector = apply_cnot(statevector, num_qubit, 0, 2)
    print("After CNOT gate (control: 0, target: 2):")
    print_statevector(statevector, num_qubit)

    # Runtime testing
    max_qubits = 28
    for num_qubit in range(1, max_qubits + 1):
        time_taken = test_runtime(num_qubit)
        save_runtime_data(num_qubit, time_taken)

if __name__ == "__main__":
    main()