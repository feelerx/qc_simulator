# qc_simulator

Implemented a simple simulator using 3 approaches in both C and python.

Every approach should be run differently. 

For C, compile with the usual "clang filename.c headername.h" for each approach.

For Python, compile with the usual "python3 filename.py" for each approach.

After compilation, a runtime.txt file will be automatically generated.

When the runtime.txt file is generated, run "python plot.py" to see a graph of qubits and time taken for the approach to simulate it.
(The runtime.txt file should be in thesame folder as the plot.py file) 

For the Matrix_mult, we see that around 14 qubits can be simulated before results start shooting up.

For the Tensor_mult, we see that number improve to around 28,29 qubits.

We also see that all the C approaches perform way better than their Python counterpart. 

Future Possible Work:
Try adding noise models, and state visualization to the simple simulator

Try parallelizing the matrix and tensor multiplication for both approaches
Or check for other ways to optimize the code

