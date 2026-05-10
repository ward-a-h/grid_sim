#include "grid.h"
#include <time.h>

void* fault_thread(void* arg) {

    GridState* grid = (GridState*) arg;

    srand(time(NULL));

    const char* names[] = {
        "COAL",
        "SOLAR",
        "WIND"
    };

    while (grid->stop == 0) {

        // wait before creating fault
        int wait_before_fault = rand() % 10 + 8;
        sleep(wait_before_fault);

        // stop safely if simulation ended
        if (grid->stop == 1) {
            break;
        }

        // choose random generator
        int target = rand() % NUM_GENERATORS;

        pthread_mutex_lock(&grid->lock);

        // avoid faulting already broken generator
        if (grid->generator_active[target] == 1) {

            grid->generator_active[target] = 0;

            // metrics update
            grid->metrics.fault_count++;

            printf(RED
                   "\n[FAULT] %s generator went DOWN!\n\n"
                   RESET,
                   names[target]);

            fflush(stdout);

            pthread_cond_broadcast(&grid->demand_change);
        }

        pthread_mutex_unlock(&grid->lock);

        // recovery delay
        int recovery_time = rand() % 8 + 5;
        sleep(recovery_time);

        // stop safely if simulation ended
        if (grid->stop == 1) {
            break;
        }

        pthread_mutex_lock(&grid->lock);

        // restore generator only if simulation still running
        if (grid->stop == 0) {

            grid->generator_active[target] = 1;

            // metrics update
            grid->metrics.recovery_count++;

            printf(GREEN
                   "\n[RECOVERY] %s generator is back ONLINE after %d seconds\n\n"
                   RESET,
                   names[target],
                   recovery_time);

            fflush(stdout);

            pthread_cond_broadcast(&grid->demand_change);
        }

        pthread_mutex_unlock(&grid->lock);
    }

    return NULL;
}
