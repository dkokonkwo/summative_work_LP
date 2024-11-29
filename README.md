Qn1. System Resource Monitoring (5)

Assessment Goal:

Interact with Operating System API
Make use of System Calls
Manipulate files
Use Python features in C code
Context

Everyone would love to know to what extent the computer under use make use of the CPU, Memory, Disk and Network Usage. In this project you are required to develop such a tool that will be monitoring CPU usage, Memory usage, and Network Usage (received and transmitted bandwidth).

Upon executing the program, the percentage (CPU and Memory) and the timestamp (hh:mm:ss) as well as Network usage (KB ) have to be recorded in a file (.txt)on the disk with a delay of 2 seconds recapture.

Tasks

Track CPU usage (%), Memory usage (%), and Network Usage (KB)
Record the metrics in a text file (usage.txt) on the disk
List all active processes (in processes.txt file)
Write a CPython that will read all metrics from the usage.txt file and plot the values in a graph
Q2. Multi-Threading (6)

Assessment Goals

Learn Thread Creation and Usage
Learn Thread Synchronization
Context

Factories use producer-consumer model a lot, in company that produces mineral water we have two arms. The first arm fills the bottle with water (producer) while the second arm seal the bottle and remove it from queue (consumer). You will develop a program that simulates this producer consumer model.

Tasks:

There will be a thread (producer) that will be generating bottles and places them on the queue with a delay of 2 seconds.
There will be another thread (consumer) that will be taking the bottles off the queue with a delay of 3 seconds.
The consumer will only work if there is a bottle on the queue.
The queue can only handle 10 bottles (if the queue reaches 10 bottles, the producer will stop while the consumer will taking the bottles off. as long as the bottles on the queue go below the 10, the producer will resume its work)
The producer will be incrementing the number of bottles on the queue while the consumer will be decrementing the number of bottles.
Q3. Socket (6)

Assessment Goals

Implement server
Implement client
Authenticate clients and ensure safe communication
Context

We are interested in developing a chat system in which we have a central server that manages 4 clients. Every client will login with the credential (username) before starts chatting with other online clients. Anytime the client authenticates with their username, they should be able to see all clients online and choose who to chat with, once the client choose the other client to chat, they can send a message which will be received on the other side as the message sender and the actual message. That is to say, that the clients can only communicate if they both connected and authenticated.

Tasks

Implement the server
Implement 4 clients
Ensure that chats are bidirectional (either side can send but also can receive)
Ensure that only connected and authenticated clients can communicate
Q4. IP Capture (5)

Assessment Goal

Track IP addresses in the same network (use client)
Send valid IP addresses to the server
Log IP addresses in a text file using the server.
Context

The network admin wants to have a list of all devices connected the network with intention to track any computer on the network that might external. Based on this need, you are required to develop a program that will return all IP addresses and and their respective hostnames for computers connected to the network.

Tasks:

Complete a client-server application 
The client will return all IP addresses and Hostnames for all nodes in the same network (minimum two devices)
The server will log all IP addresses and hostnames in a text file.
