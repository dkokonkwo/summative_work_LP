Qn1. System Resource Monitoring (5)

Assessment Goal:

- Interact with Operating System API
- Make use of System Calls
- Manipulate files
- Use Python features in C code

Context

Everyone would love to know to what extent the computer under use make use of the CPU, Memory, Disk and Network Usage. In this project you are required to develop such a tool that will be monitoring CPU usage, Memory usage, and Network Usage (received and transmitted bandwidth).

Upon executing the program, the percentage (CPU and Memory) and the timestamp (hh:mm:ss) as well as Network usage (KB ) have to be recorded in a file (.txt)on the disk with a delay of 2 seconds recapture.

Tasks

- Track CPU usage (%), Memory usage (%), and Network Usage (KB)
- Record the metrics in a text file (usage.txt) on the disk
- List all active processes (in processes.txt file)
- Write a CPython that will read all metrics from the usage.txt file and plot the values in a graph