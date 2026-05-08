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

    while (1) {

        // wait before creating fault
        int wait_before_fault = rand() % 10 + 8;
        sleep(wait_before_fault);

        // choose random generator
        int target = rand() % NUM_GENERATORS;

        pthread_mutex_lock(&grid->lock);

        // avoid faulting already broken generator
        if (grid->generator_active[target] == 1) {

            grid->generator_active[target] = 0;

            printf("\n[FAULT] %s generator DOWN\n\n",
                   names[target]);

            fflush(stdout);

            pthread_cond_broadcast(&grid->demand_change);
        }

        pthread_mutex_unlock(&grid->lock);

        // recovery delay
        int recovery_time = rand() % 8 + 5;
        sleep(recovery_time);

        pthread_mutex_lock(&grid->lock);

        grid->generator_active[target] = 1;

        printf("\n[RECOVERY] %s generator restored after %d seconds\n\n",
               names[target],
               recovery_time);

        fflush(stdout);

        pthread_cond_broadcast(&grid->demand_change);

        pthread_mutex_unlock(&grid->lock);
    }

    return NULL;
}
