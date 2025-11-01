# Quantum Circuit Simulator

A simple quantum circuit simulator implementing three different approaches in both C and Python for efficient comparative analysis of quantum circuit simulation performance.

## Overview

This project provides implementations of quantum circuit simulation using three distinct computational approaches:
- Matrix multiplication
- Tensor multiplication  
- Additional optimization approach

Each approach is implemented in both C and Python to enable direct performance comparison between the two languages.

## Features

- Three simulation approaches with varying qubit capacity
- Dual implementation in C and Python
- Automatic performance benchmarking
- Visual performance comparison via plotting
- Runtime analysis across different qubit counts

## Performance Characteristics

- **Matrix multiplication approach**: Effectively simulates up to approximately 14 qubits before performance degrades
- **Tensor multiplication approach**: Improved scalability, handling approximately 28-29 qubits
- **C implementations**: Significantly outperform their Python counterparts across all approaches

## Requirements

### C Implementation
- C compiler (clang recommended)

### Python Implementation
- Python 3.x
- Required packages for plotting

## Installation

Clone the repository:
```bash
git clone https://github.com/feelerx/qc_simulator.git
cd qc_simulator
```

## Usage

### Running C Implementations

Compile each approach using:
```bash
clang filename.c headername.h
```

Execute the compiled binary to run the simulation.

### Running Python Implementations

Execute each approach using:
```bash
python3 filename.py
```

### Generating Performance Visualizations

After running any implementation:

1. A `runtime.txt` file will be automatically generated
2. Ensure `runtime.txt` is in the same directory as `plot.py`
3. Generate the performance graph:
```bash
python plot.py
```

The graph displays the relationship between the number of qubits and the time taken to simulate them for each approach.

## Project Structure

```
qc_simulator/
├── C implementations
├── Python implementations
├── plot.py          # Performance visualization script
└── runtime.txt      # Generated runtime data (auto-created)
```

## Future Work

Potential improvements and extensions:
- Implementation of noise models for realistic quantum simulation
- State visualization capabilities
- Parallelization of matrix and tensor multiplication operations
- Further code optimization techniques
- Additional simulation approaches

## Contributing

Contributions are welcome. Please feel free to submit issues and pull requests.

---

Note: This README is based on the available repository information. Some sections may need to be updated with additional details about specific file names, dependencies, and usage examples as the project evolves.
