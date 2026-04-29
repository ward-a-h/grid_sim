#include "grid.h"
#include <time.h>

void* residential_consumer(void* arg) {
    srand(time(NULL));

    GridState* grid = (GridState*) arg;
    int id = 0; // residential is index 0

    while (1) {
        // sleep BEFORE locking
        sleep(3);

        // random demand between 60 and 100 units
        int demand = rand() % 41 + 60;

        // enter critical section — no other thread can touch grid data now
        pthread_mutex_lock(&grid->lock);

        // tell the grid what we need
        grid->region_demand[id] = demand;

        // if there isn't enough energy, wait safely
        // pthread_cond_wait releases the lock and sleeps atomically
        // when woken up, it re-acquires the lock automatically
        while (grid->current_load < demand) {
            printf("RESIDENTIAL: needs %d units, grid only has %d. Waiting...\n",
                   demand, grid->current_load);
            pthread_cond_wait(&grid->demand_change, &grid->lock);
        }

        // enough energy is available — consume it
        grid->current_load -= demand;
        grid->region_served[id] += demand;

        printf("RESIDENTIAL: consumed %d units. Grid now at %d/%d\n",
               demand, grid->current_load, MAX_CAPACITY);

        // wake up other threads (generators or other consumers) about the change
        pthread_cond_broadcast(&grid->demand_change);

        // release the lock
        pthread_mutex_unlock(&grid->lock);
    }
    return NULL;
}

void* industrial_consumer(void* arg) {
    GridState* grid = (GridState*) arg;
    int id = 1; // industrial is index 1

    while (1) {
        sleep(4); // industrial runs on a slower cycle

        int demand = 80; // always steady, no randomness

        pthread_mutex_lock(&grid->lock);

        grid->region_demand[id] = demand;

        while (grid->current_load < demand) {
            printf("INDUSTRIAL: needs %d units, grid only has %d. Waiting...\n",
                   demand, grid->current_load);
            pthread_cond_wait(&grid->demand_change, &grid->lock);
        }

        grid->current_load -= demand;
        grid->region_served[id] += demand;

        printf("INDUSTRIAL: consumed %d units. Grid now at %d/%d\n",
               demand, grid->current_load, MAX_CAPACITY);

        pthread_cond_broadcast(&grid->demand_change);
        pthread_mutex_unlock(&grid->lock);
    }
    return NULL;
}

void* commercial_consumer(void* arg) {
    GridState* grid = (GridState*) arg;
    int id = 2; // commercial is index 2

    while (1) {
        sleep(5); // slowest cycle

        int demand = rand() % 21 + 40; // 40 to 60 units

        pthread_mutex_lock(&grid->lock);

        grid->region_demand[id] = demand;

        while (grid->current_load < demand) {
            printf("COMMERCIAL: needs %d units, grid only has %d. Waiting...\n",
                   demand, grid->current_load);
            pthread_cond_wait(&grid->demand_change, &grid->lock);
        }

        grid->current_load -= demand;
        grid->region_served[id] += demand;

        printf("COMMERCIAL: consumed %d units. Grid now at %d/%d\n",
               demand, grid->current_load, MAX_CAPACITY);

        pthread_cond_broadcast(&grid->demand_change);
        pthread_mutex_unlock(&grid->lock);
    }
    return NULL;
}








