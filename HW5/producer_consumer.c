#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 10
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2
#define ITEMS_TO_PRODUCE 20

typedef struct {
    int buffer[BUFFER_SIZE];
    int count;
    int in;
    int out;
    pthread_mutex_t lock;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} Buffer;

Buffer buf;
int items_produced = 0;
int items_consumed = 0;
pthread_mutex_t done_lock;

void *producer(void *arg) {
    int id = *(int *)arg;
    while (1) {
        pthread_mutex_lock(&buf.lock);

        while (buf.count == BUFFER_SIZE) {
            pthread_cond_wait(&buf.not_full, &buf.lock);
        }

        pthread_mutex_lock(&done_lock);
        if (items_produced >= ITEMS_TO_PRODUCE) {
            pthread_mutex_unlock(&done_lock);
            pthread_mutex_unlock(&buf.lock);
            break;
        }
        int item = items_produced;
        items_produced++;
        pthread_mutex_unlock(&done_lock);

        buf.buffer[buf.in] = item;
        printf("[Producer %d] Produced item %d (buffer: %d/%d)\n",
               id, item, buf.count + 1, BUFFER_SIZE);
        buf.in = (buf.in + 1) % BUFFER_SIZE;
        buf.count++;

        pthread_cond_signal(&buf.not_empty);
        pthread_mutex_unlock(&buf.lock);

        usleep(rand() % 500000);
    }
    printf("[Producer %d] Done\n", id);
    return NULL;
}

void *consumer(void *arg) {
    int id = *(int *)arg;
    while (1) {
        pthread_mutex_lock(&buf.lock);

        while (buf.count == 0) {
            pthread_mutex_lock(&done_lock);
            if (items_consumed >= ITEMS_TO_PRODUCE) {
                pthread_mutex_unlock(&done_lock);
                pthread_mutex_unlock(&buf.lock);
                return NULL;
            }
            pthread_mutex_unlock(&done_lock);
            pthread_cond_wait(&buf.not_empty, &buf.lock);
        }

        int item = buf.buffer[buf.out];
        buf.out = (buf.out + 1) % BUFFER_SIZE;
        buf.count--;
        printf("[Consumer %d] Consumed item %d (buffer: %d/%d)\n",
               id, item, buf.count, BUFFER_SIZE);

        pthread_mutex_lock(&done_lock);
        items_consumed++;
        pthread_mutex_unlock(&done_lock);

        pthread_cond_signal(&buf.not_full);
        pthread_mutex_unlock(&buf.lock);

        usleep(rand() % 700000);
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    pthread_mutex_init(&buf.lock, NULL);
    pthread_cond_init(&buf.not_full, NULL);
    pthread_cond_init(&buf.not_empty, NULL);
    pthread_mutex_init(&done_lock, NULL);

    buf.count = 0;
    buf.in = 0;
    buf.out = 0;

    pthread_t producers[NUM_PRODUCERS], consumers[NUM_CONSUMERS];
    int pids[NUM_PRODUCERS], cids[NUM_CONSUMERS];

    printf("=== Producer-Consumer Problem ===\n");
    printf("Buffer size: %d, Producers: %d, Consumers: %d\n\n",
           BUFFER_SIZE, NUM_PRODUCERS, NUM_CONSUMERS);

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pids[i] = i + 1;
        pthread_create(&producers[i], NULL, producer, &pids[i]);
    }
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        cids[i] = i + 1;
        pthread_create(&consumers[i], NULL, consumer, &cids[i]);
    }

    for (int i = 0; i < NUM_PRODUCERS; i++)
        pthread_join(producers[i], NULL);
    for (int i = 0; i < NUM_CONSUMERS; i++)
        pthread_join(consumers[i], NULL);

    printf("\nAll items produced: %d, consumed: %d\n",
           items_produced, items_consumed);

    pthread_mutex_destroy(&buf.lock);
    pthread_cond_destroy(&buf.not_full);
    pthread_cond_destroy(&buf.not_empty);
    pthread_mutex_destroy(&done_lock);
    return 0;
}
