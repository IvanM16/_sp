#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5
#define MEALS 3

pthread_mutex_t chopsticks[NUM_PHILOSOPHERS];

void think(int id) {
    printf("Philosopher %d is thinking...\n", id);
    usleep(rand() % 1000000);
}

void eat(int id) {
    printf("Philosopher %d is eating...\n", id);
    usleep(rand() % 1000000);
}

void *philosopher(void *arg) {
    int id = *(int *)arg;
    int left = id;
    int right = (id + 1) % NUM_PHILOSOPHERS;

    for (int i = 0; i < MEALS; i++) {
        think(id);

        if (id == NUM_PHILOSOPHERS - 1) {
            pthread_mutex_lock(&chopsticks[right]);
            printf("Philosopher %d picked up right chopstick %d\n", id, right);
            pthread_mutex_lock(&chopsticks[left]);
            printf("Philosopher %d picked up left chopstick %d\n", id, left);
        } else {
            pthread_mutex_lock(&chopsticks[left]);
            printf("Philosopher %d picked up left chopstick %d\n", id, left);
            pthread_mutex_lock(&chopsticks[right]);
            printf("Philosopher %d picked up right chopstick %d\n", id, right);
        }

        eat(id);

        pthread_mutex_unlock(&chopsticks[left]);
        printf("Philosopher %d put down left chopstick %d\n", id, left);
        pthread_mutex_unlock(&chopsticks[right]);
        printf("Philosopher %d put down right chopstick %d\n", id, right);
    }
    printf("Philosopher %d finished all meals\n", id);
    return NULL;
}

int main() {
    srand(time(NULL));
    pthread_t philosophers[NUM_PHILOSOPHERS];
    int ids[NUM_PHILOSOPHERS];

    for (int i = 0; i < NUM_PHILOSOPHERS; i++)
        pthread_mutex_init(&chopsticks[i], NULL);

    printf("=== Dining Philosophers Problem ===\n");
    printf("Philosophers: %d, Meals each: %d\n", NUM_PHILOSOPHERS, MEALS);
    printf("Deadlock prevention: Philosopher %d picks right first\n\n",
           NUM_PHILOSOPHERS);

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &ids[i]);
    }
    for (int i = 0; i < NUM_PHILOSOPHERS; i++)
        pthread_join(philosophers[i], NULL);

    printf("\nAll philosophers finished. No deadlock!\n");

    for (int i = 0; i < NUM_PHILOSOPHERS; i++)
        pthread_mutex_destroy(&chopsticks[i]);
    return 0;
}
