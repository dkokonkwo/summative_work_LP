import sysmonitor
import matplotlib.pyplot as plt
from datetime import datetime

runtime = 10
print(f"Running process {runtime} times...")

sysmonitor.sysmon(runtime)
print("Data logging complete.")

# Parse the data
timestamps = []
cpu_usages = []
memory_usages = []
network_usages = []

with open("usage.txt", "r") as file:
    lines = file.readlines()

# Parse the data by iterating through lines
for i in range(0, len(lines), 4):
    ts = lines[i].split(": ", 1)[1].strip()
    cpu = lines[i + 1].split(": ", 1)[1].strip('%\n')
    memory = lines[i + 2].split(": ", 1)[1].strip('%\n')
    network = lines[i + 3].split(": ", 1)[1].strip(' KB\n')

    timestamps.append(datetime.strptime(ts, "%a %b %d %H:%M:%S %Y"))
    cpu_usages.append(float(cpu))
    memory_usages.append(float(memory))
    network_usages.append(int(network))

# print(timestamps)
# print(cpu_usages)
# print(memory_usages)
# print(network_usages)

# Normalizing values to fit in graph
def normalize(data, decimals=2):
    min_val = min(data)
    max_val = max(data)
    return [round((x - min_val) / (max_val - min_val), 2) for x in data]

normalized_cpu = normalize(cpu_usages)
normalized_memory = normalize(memory_usages)
normalized_network = normalize(network_usages)

# print(normalized_cpu)
# print(normalized_memory)
# print(normalized_network)

# Plot the data
plt.figure(figsize=(12, 6))

# Plot CPU usage
plt.plot(timestamps, normalized_cpu, label="CPU Usage (%)", marker='o')

# Plot Memory usage
plt.plot(timestamps, normalized_memory, label="Memory Usage (%)", marker='x')

# Plot Network usage (scaled for visualization)
plt.plot(timestamps, normalized_network, label="Network Usage (KB)", marker='s', linestyle='--')

# Customize the plot
plt.xlabel("Time")
plt.ylabel("Usage")
plt.title("System Resource Usage Over Time")
plt.legend()
plt.grid(True)

# Rotate x-axis labels for better readability
plt.xticks(rotation=45)
plt.tight_layout()

# Show the plot
# plt.show()

# Saving the plot
plt.savefig("resource_usage.png")