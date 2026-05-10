#include "grid.h"
#include <time.h>

// Clock thread — ticks every 5 seconds, drives peak/off-peak logic
void* clock_thread(void* arg) {
    GridState* grid = (GridState*) arg;
    while (grid->stop == 0) {
        sleep(5);
        if (grid->stop == 1) break;
        pthread_mutex_lock(&grid->lock);
        grid->tick++;
        printf(CYAN "[CLOCK] Tick %d — %s\n" RESET,
               grid->tick,
               (grid->tick % 8 >= 4) ? "PEAK HOURS" : "off-peak");
        pthread_cond_broadcast(&grid->demand_change);
        pthread_mutex_unlock(&grid->lock);
    }
    return NULL;
}

void* residential_consumer(void* arg) {
    srand(time(NULL));
    GridState* grid = (GridState*) arg;
    int id = 0;

    while (grid->stop == 0) {

        // check peak hours before sleeping
        pthread_mutex_lock(&grid->lock);
        int is_peak = (grid->tick % 8 >= 4);
        pthread_mutex_unlock(&grid->lock);

        // peak hours: faster cycle and higher demand
        int cycle_sleep = is_peak ? 2 : 3;
        int demand = is_peak ? (rand() % 41 + 120) : (rand() % 41 + 60);

        if (is_peak) {
            printf(BLUE "RESIDENTIAL: PEAK HOURS — high demand incoming\n" RESET);
        }

        sleep(cycle_sleep);

        pthread_mutex_lock(&grid->lock);

        if (grid->stop == 1) {
            pthread_mutex_unlock(&grid->lock);
            break;
        }

        grid->region_demand[id] = demand;

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
            pthread_cond_broadcast(&grid->demand_change);
        }

        pthread_mutex_unlock(&grid->lock);
    }
    return NULL;
}

void* industrial_consumer(void* arg) {
    GridState* grid = (GridState*) arg;
    int id = 1;

    while (grid->stop == 0) {
        sleep(4);

        pthread_mutex_lock(&grid->lock);

        if (grid->stop == 1) {
            pthread_mutex_unlock(&grid->lock);
            break;
        }

        int demand = 80;
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
    int id = 2;

    while (grid->stop == 0) {

        // commercial is only active during off-peak (daytime) hours
        pthread_mutex_lock(&grid->lock);
        int is_daytime = (grid->tick % 8 < 5);
        pthread_mutex_unlock(&grid->lock);

        if (!is_daytime) {
            printf(BLUE "COMMERCIAL: off-hours, not requesting energy.\n" RESET);
            sleep(5);
            continue;
        }

        sleep(5);

        pthread_mutex_lock(&grid->lock);

        if (grid->stop == 1) {
            pthread_mutex_unlock(&grid->lock);
            break;
        }

        int demand = rand() % 21 + 40;
        grid->region_demand[id] = demand;

        while (grid->current_load < demand && grid->stop == 0) {
            printf(BLUE "COMMERCIAL: needs %d units, grid only has %d. Waiting...\n" RESET,
                   demand, grid->current_load);
            pthread_cond_wait(&grid->demand_change, &grid->lock);
        }

        if (grid->stop == 0) {
            balance_load(grid);
            printf(BLUE "COMMERCIAL: request processed.\n" RESET);
            grid->region_demand[id] = 0;
            pthread_cond_broadcast(&grid->demand_change);
        }

        pthread_mutex_unlock(&grid->lock);
    }
    return NULL;
}
