# qc_simulator

Implemented the simple simulator using 3 approaches.

Every approach should be run differently. 

Compile with the usual "clang statevector.c" for each approach.

After compilation, a runtime.txt file will be automatically generated.

When the file is created, run "python plot.py" to see a graph of qubits and time taken for the approach to simulate it.

For the Matrix_mult, we see that around 14 qubits can be simulated before results start shooting up.

For the Tensor_mult, we see that number improve to around 30 qubits.


Future Possible Work:
Try parallelizing the matrix and tensor multiplication for both approaches
Or check for other ways to optimize the code

