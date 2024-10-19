import matplotlib.pyplot as plt

# Read the data from the file
qubits = []
runtimes = []

with open('runtime_data.txt', 'r') as f:
    for line in f:
        data = line.split()
        qubits.append(int(data[0]))
        runtimes.append(float(data[1]))

# Plotting the data
plt.plot(qubits, runtimes, marker='o')
plt.xlabel('Number of Qubits')
plt.ylabel('Runtime (seconds)')
plt.title('Runtime vs Number of Qubits')
plt.grid(True)
plt.show()
