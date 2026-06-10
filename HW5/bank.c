#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 4
#define OPS_PER_THREAD 25000

typedef struct {
    long long balance;
    pthread_mutex_t lock;
    int use_mutex;
} Account;

Account account;

void *do_transactions(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < OPS_PER_THREAD; i++) {
        if (account.use_mutex) pthread_mutex_lock(&account.lock);

        account.balance += 100;
        account.balance -= 100;

        if (account.use_mutex) pthread_mutex_unlock(&account.lock);
    }
    printf("Thread %d finished %d transactions\n", id, OPS_PER_THREAD);
    return NULL;
}

void run_simulation(int use_mutex) {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    account.balance = 0;
    account.use_mutex = use_mutex;

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, do_transactions, &ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final balance: %lld (expected 0)\n", account.balance);
    if (account.balance == 0)
        printf("SUCCESS: Balance is correct!\n\n");
    else
        printf("ERROR: Race condition caused incorrect balance!\n\n");
}

int main() {
    pthread_mutex_init(&account.lock, NULL);

    printf("=== Bank Simulation: %d threads, %d ops/thread ===\n",
           NUM_THREADS, OPS_PER_THREAD);

    printf("\n--- WITHOUT mutex (race condition) ---\n");
    run_simulation(0);

    sleep(1);

    printf("--- WITH mutex (thread-safe) ---\n");
    run_simulation(1);

    pthread_mutex_destroy(&account.lock);
    return 0;
}
