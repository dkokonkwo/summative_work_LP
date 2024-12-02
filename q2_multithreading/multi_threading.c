#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define QUEUE_CAPACITY 10

int bottle_queue = 0;                 // Number of bottles in the queue
pthread_mutex_t mutex;                // Mutex for synchronization
pthread_cond_t not_full;              // Condition variable for "queue not full"
pthread_cond_t not_empty;             // Condition variable for "queue not empty"

// Producer thread function
void *producer(void *arg) {
    while (1) {
        sleep(2);  // Simulate the production time

        pthread_mutex_lock(&mutex);

        while (bottle_queue == QUEUE_CAPACITY) {
            printf("Producer: Queue is full, waiting...\n");
            pthread_cond_wait(&not_full, &mutex);  // Wait until queue has space
        }

        bottle_queue++;  // Add a bottle to the queue
        printf("Producer: Produced a bottle, queue size: %d\n", bottle_queue);

        pthread_cond_signal(&not_empty);  // Notify consumer that a bottle is available
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

// Consumer thread function
void *consumer(void *arg) {
    while (1) {
        sleep(3);  // Simulate the consumption time

        pthread_mutex_lock(&mutex);

        while (bottle_queue == 0) {
            printf("Consumer: Queue is empty, waiting...\n");
            pthread_cond_wait(&not_empty, &mutex);  // Wait until queue has bottles
        }

        bottle_queue--;  // Remove a bottle from the queue
        printf("Consumer: Consumed a bottle, queue size: %d\n", bottle_queue);

        pthread_cond_signal(&not_full);  // Notify producer that space is available
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main() {
    pthread_t producer_thread, consumer_thread;

    // Initialize mutex and condition variables
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&not_empty, NULL);

    // Create producer and consumer threads
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    // Wait for both threads to finish
    pthread_join(producer_thread, NULL);  // This will run indefinitely
    pthread_join(consumer_thread, NULL); // This will run indefinitely

    // Destroy mutex and condition variables
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);

    return 0;
}
