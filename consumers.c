#include "grid.h"
#include <time.h>

void* clock_thread(void* arg) {
    GridState* grid = (GridState*) arg;
    while (grid->stop == 0) {
        sleep(5);
        if (grid->stop == 1) break;
        pthread_mutex_lock(&grid->lock);
        grid->tick++;
        printf(BOLD "CLOCK: tick %d\n" RESET, grid->tick);
        pthread_mutex_unlock(&grid->lock);
    }
    return NULL;
}

void* residential_consumer(void* arg) {
    srand(time(NULL));
    GridState* grid = (GridState*) arg;
    int id = 0; // residential is index 0

    while (grid->stop == 0) {

        // read tick to decide peak or normal hours
        pthread_mutex_lock(&grid->lock);
        int current_tick = grid->tick;
        pthread_mutex_unlock(&grid->lock);

        // peak hours = ticks 4-7 of every 8-tick cycle (evening simulation)
        int is_peak = (current_tick % 8 >= 4);

        int demand;
        int cycle_sleep;

        if (is_peak) {
            demand = rand() % 41 + 120; // 120-160 units during peak
            cycle_sleep = 2;             // requests come faster during peak
            printf(BLUE "RESIDENTIAL: PEAK HOURS - high demand period\n" RESET);
        } else {
            demand = rand() % 41 + 60;  // random demand between 60 and 100 units
            cycle_sleep = 3;
        }

        // sleep BEFORE locking
        sleep(cycle_sleep);

        // enter critical section — no other thread can touch grid data now
        pthread_mutex_lock(&grid->lock);

        if (grid->stop == 1) {
            pthread_mutex_unlock(&grid->lock);
            break;
        }

        // tell the grid what we need
        grid->region_demand[id] = demand;

        // if there isn't enough energy, wait safely
        // pthread_cond_wait releases the lock and sleeps atomically
        // when woken up, it re-acquires the lock automatically
        while (grid->current_load < demand && grid->stop == 0) {
            printf(BLUE "RESIDENTIAL: needs %d units, grid only has %d. Waiting...\n" RESET,
                   demand, grid->current_load);
            pthread_cond_wait(&grid->demand_change, &grid->lock);
        }

        if (grid->stop == 0) {
            balance_load(grid);
            printf(BLUE "RESIDENTIAL: request processed (%s).\n" RESET,
                   is_peak ? "PEAK" : "normal");
            grid->region_demand[id] = 0;
            // wake up other threads (generators or other consumers) about the change
            pthread_cond_broadcast(&grid->demand_change);
        }

        // release the lock
        pthread_mutex_unlock(&grid->lock);
    }
    return NULL;
}

void* industrial_consumer(void* arg) {
    GridState* grid = (GridState*) arg;
    int id = 1; // industrial is index 1

    while (grid->stop == 0) {
        sleep(4); // industrial runs on a slower cycle

        int demand = 80; // always steady, no randomness

        pthread_mutex_lock(&grid->lock);

        if (grid->stop == 1) {
            pthread_mutex_unlock(&grid->lock);
            break;
        }

        grid->region_demand[id] = demand;

        while (grid->current_load < demand && grid->stop == 0) {
            printf(BLUE "INDUSTRIAL: needs %d units, grid only has %d. Waiting...\n" RESET,
                   demand, grid->current_load);
            pthread_cond_wait(&grid->demand_change, &grid->lock);
        }

        if (grid->stop == 0) {
            balance_load(grid);
            printf(BLUE "INDUSTRIAL: request processed.\n" RESET);
            grid->region_demand[id] = 0;
            pthread_cond_broadcast(&grid->demand_change);
        }

        pthread_mutex_unlock(&grid->lock);
    }
    return NULL;
}

void* commercial_consumer(void* arg) {
    GridState* grid = (GridState*) arg;
    int id = 2; // commercial is index 2

    while (grid->stop == 0) {

        // read tick to decide if daytime or off-hours
        pthread_mutex_lock(&grid->lock);
        int current_tick = grid->tick;
        pthread_mutex_unlock(&grid->lock);

        // commercial only active during daytime ticks (0-4 of every 8-tick cycle)
        int is_daytime = (current_tick % 8 < 5);

        if (!is_daytime) {
            printf(BLUE "COMMERCIAL: closed during off-hours, sleeping...\n" RESET);
            sleep(5);
            continue;
        }

        sleep(5); // slowest cycle

        // enter critical section — no other thread can touch grid data now
        pthread_mutex_lock(&grid->lock);

        if (grid->stop == 1) {
            pthread_mutex_unlock(&grid->lock);
            break;
        }

        int demand = rand() % 21 + 40; // 40 to 60 units

        // tell the grid what we need
        grid->region_demand[id] = demand;

        // if there isn't enough energy, wait safely
        // pthread_cond_wait releases the lock and sleeps atomically
        // when woken up, it re-acquires the lock automatically
        while (grid->current_load < demand && grid->stop == 0) {
            printf(BLUE "COMMERCIAL: needs %d units, grid only has %d. Waiting...\n" RESET,
                   demand, grid->current_load);
            pthread_cond_wait(&grid->demand_change, &grid->lock);
        }

        if (grid->stop == 0) {
            balance_load(grid);
            printf(BLUE "COMMERCIAL: request processed.\n" RESET);
            grid->region_demand[id] = 0;
            // wake up other threads (generators or other consumers) about the change
            pthread_cond_broadcast(&grid->demand_change);
        }

        // release the lock
        pthread_mutex_unlock(&grid->lock);
    }
    return NULL;
}
