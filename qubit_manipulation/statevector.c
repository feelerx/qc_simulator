#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void apply_x(double *statevector, int num_qubit, int target);
void apply_h(double *statevector, int num_qubit, int target);
void apply_cnot(double *statevector, int num_qubit, int control, int target);
void print_statevector(double *statevector, int num_qubit);
void initialize_statevector(double *statevector, int num_qubit);
void save_runtime_data(int num_qubits, double time_taken);
double test_runtime(int num_qubit);

int main(void) {

    int num_qubit = 4;
    double* statevector = (double*) malloc((1 << num_qubit) * sizeof(double));

    for (int i = 0; i < (1 << num_qubit); i++) {
        statevector[i] = (i == 0) ? 1.0 : 0.0;
    }

    print_statevector(statevector, num_qubit);

    // Apply X gate on qubit 0
    apply_x(statevector, num_qubit, 2);
    print_statevector(statevector, num_qubit);

    // Apply H gate on qubit 1
    apply_h(statevector, num_qubit, 1);
    print_statevector(statevector, num_qubit);

    // Apply CNOT gate (control qubit 0, target qubit 1)
    apply_cnot(statevector, num_qubit, 0, 2);
    print_statevector(statevector, num_qubit);
    

    //TESTING AREA

    // int max_qubits = 25;  // Maximum number of qubits to simulate

    // for (int num_qubit = 1; num_qubit <= max_qubits; num_qubit++) {
    //     double time_taken = test_runtime(num_qubit);  // Test runtime for current number of qubits
    //     save_runtime_data(num_qubit, time_taken);     // Save the result to file
    // }


    free(statevector);


    return 0;
}


// Function to apply the X (Pauli-X) gate on a target qubit
void apply_x(double *statevector, int num_qubit, int target) {
    int dim = 1 << num_qubit;  // 2^num_qubit
    for (int i = 0; i < dim; i++) {
        // Check if the target qubit is 0, then swap with the state where target is 1
        if (!(i & (1 << target))) {
            int swap_index = i | (1 << target);  // Set target qubit to 1
            double temp = statevector[i];
            statevector[i] = statevector[swap_index];
            statevector[swap_index] = temp;
        }
    }
}

// Function to apply the H (Hadamard) gate on a target qubit
void apply_h(double *statevector, int num_qubit, int target) {
    int dim = 1 << num_qubit;  // 2^num_qubit
    double sqrt_2_inv = 1.0 / sqrt(2.0);  // Precompute 1/sqrt(2)
    
    for (int i = 0; i < dim; i++) {
        // Check if the target qubit is 0, then combine the states where target is 0 and 1
        if (!(i & (1 << target))) {
            int target_1_index = i | (1 << target);  // Set target qubit to 1
            double a = statevector[i];  // Amplitude where target qubit is 0
            double b = statevector[target_1_index];  // Amplitude where target qubit is 1

            // Update the amplitudes according to Hadamard transformation
            statevector[i] = sqrt_2_inv * (a + b);
            statevector[target_1_index] = sqrt_2_inv * (a - b);
        }
    }
}

// Function to apply the CNOT gate (as described earlier)
void apply_cnot(double *statevector, int num_qubit, int control, int target) {
    int dim = 1 << num_qubit;  // 2^num_qubit
    for (int i = 0; i < dim; i++) {
        // Check if control qubit is 1
        if (i & (1 << control)) {
            // Get the indices for the target qubit being 0 or 1
            int target0 = i & ~(1 << target);  // target = 0
            int target1 = i | (1 << target);   // target = 1
            
            // Swap amplitudes for target qubit being 0 and 1
            double temp = statevector[target0];
            statevector[target0] = statevector[target1];
            statevector[target1] = temp;
        }
    }
}

// Function to print the current statevector
void print_statevector(double *statevector, int num_qubit) {
    int dim = 1 << num_qubit;  // 2^num_qubit
    for (int i = 0; i < dim; i++) {
        printf("state[%d] = %f\n", i, statevector[i]);
    }
        printf("\n");
}   

void initialize_statevector(double *statevector, int num_qubit) {
    int dim = 1 << num_qubit;
    for (int i = 0; i < dim; i++) {
        statevector[i] = (i == 0) ? 1.0 : 0.0;  // Initialize to |0...0⟩
    }
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

    initialize_statevector(statevector, num_qubit);  // Initialize to |0...0⟩

    // Start measuring time
    clock_t start_time = clock();

    // Apply gates (X, H, CNOT) on various qubits
    apply_x(statevector, num_qubit, 0);
    apply_h(statevector, num_qubit, 0);
    if (num_qubit > 1) {
        apply_cnot(statevector, num_qubit, 0, 1);  // Only if there are at least 2 qubits
    }

    // Stop measuring time
    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Free memory
    free(statevector);

    // Return the time taken
    return time_taken;
}