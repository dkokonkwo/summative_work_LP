Q2. Multi-Threading (6)

Assessment Goals

Learn Thread Creation and Usage
Learn Thread Synchronization
Context

Factories use producer-consumer model a lot, in company that produces mineral water we have two arms. The first arm fills the bottle with water (producer) while the second arm seal the bottle and remove it from queue (consumer). You will develop a program that simulates this producer consumer model.

Tasks:

- There will be a thread (producer) that will be generating bottles and places them on the queue with a delay of 2 seconds.
- There will be another thread (consumer) that will be taking the bottles off the queue with a delay of 3 seconds.
- The consumer will only work if there is a bottle on the queue.
- The queue can only handle 10 bottles (if the queue reaches 10 bottles, the producer will stop while the consumer will taking the bottles off. as long as the bottles on the queue go below the 10, the producer will resume its work)
- The producer will be incrementing the number of bottles on the queue while the consumer will be decrementing the number of bottles.