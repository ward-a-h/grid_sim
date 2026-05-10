#include "grid.h"

void* coal_generator(void* arg) {
    //arg comes in as void* so we cast it back to GridState* to actually use it
    GridState* grid = (GridState*) arg;
    //coal runs every 2 seconds, always produces the same amount
    while (grid->stop == 0) {
        sleep(2); //sleeping before lock so other threads aren't blocked while we wait
        if (grid->stop == 1) {
         break;
        }
        pthread_mutex_lock(&grid->lock);

        //check if coal plant is online before producing
        if (grid->generator_active[0]) {
            int production = 50;
            //only add to grid if there is room
            if (grid->current_load + production <= MAX_CAPACITY) {
                grid->current_load += production;
                printf(GREEN "COAL: produced %d units. Grid is now at %d out of %d\n" RESET,
                       production, grid->current_load, MAX_CAPACITY);
                //wake up any consumers that were waiting for energy
                pthread_cond_broadcast(&grid->demand_change);
            }
        } else {
            printf(RED "COAL: generator is down\n" RESET);
        }

        pthread_mutex_unlock(&grid->lock);
    }
    return NULL;
}

void* solar_generator(void* arg) {
    GridState* grid = (GridState*) arg;
    //tick keeps track of day and night cycles
    int tick = 0;
    while (grid->stop == 0) {
        sleep(3); //slightly slower production than coal
        if (grid->stop == 1) {
         break;
        }
        tick++;
        pthread_mutex_lock(&grid->lock);

        if (grid->generator_active[1]) {
            //every 6 ticks, first 4 are day and last 2 are night
            int is_daytime;
            if (tick % 6 < 4) {
                is_daytime = 1;
            } else {
                is_daytime = 0;
            }

            //solar only produces during the day
            int production;
            if (is_daytime) {
                production = 80;
            } else {
                production = 0;
            }

            if (production > 0 && grid->current_load + production <= MAX_CAPACITY) {
                grid->current_load += production;
                printf(YELLOW "SOLAR: produced %d units. Grid is now at %d out of %d\n" RESET,
                       production, grid->current_load, MAX_CAPACITY);
                pthread_cond_broadcast(&grid->demand_change);
            } else if (is_daytime == 0) {
                printf(YELLOW "SOLAR: nighttime, no production\n" RESET);
            }
        } else {
            printf(RED "SOLAR: generator is down\n" RESET);
        }

        pthread_mutex_unlock(&grid->lock);
    }
    return NULL;
}

void* wind_generator(void* arg) {
    GridState* grid = (GridState*) arg;
    //wind is unpredictable, so both timing and output are random
    while (grid->stop == 0) {
        int wait_time = rand() % 3 + 1;
        sleep(wait_time);//wind is unpredictable so rand sleep imitates that
        if (grid->stop == 1) {
         break;
        }
        pthread_mutex_lock(&grid->lock);

        if (grid->generator_active[2]) {
            //wind produces somewhere between 20 and 70 units each time
            int production = rand() % 51 + 20;
            if (grid->current_load + production <= MAX_CAPACITY) {
                grid->current_load += production;
                printf(CYAN "WIND: produced %d units. Grid is now at %d out of %d\n" RESET,
                       production, grid->current_load, MAX_CAPACITY);
                pthread_cond_broadcast(&grid->demand_change);
            }
        } else {
            printf(RED "WIND: generator is down\n" RESET);
        }

        pthread_mutex_unlock(&grid->lock);
    }
    return NULL;
}
