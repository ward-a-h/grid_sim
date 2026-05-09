#include "grid.h"
#include <time.h>

void* residential_consumer(void* arg) {
    srand(time(NULL));
    GridState* grid = (GridState*) arg;
    int id = 0;

    while (grid->stop == 0) {
        sleep(3);

        int demand = rand() % 41 + 60;

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
            printf(BLUE "RESIDENTIAL: request processed.\n" RESET);
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

        int demand = 80;

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
    int id = 2;

    while (grid->stop == 0) {
        sleep(5);

        int demand = rand() % 21 + 40;

        pthread_mutex_lock(&grid->lock);

        if (grid->stop == 1) {
            pthread_mutex_unlock(&grid->lock);
            break;
        }

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
