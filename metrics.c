#include "grid.h"

void* metrics_thread(void* arg) {

    GridState* grid = (GridState*) arg;

    int cycle = 0;

while (cycle < 2) {

        sleep(20);

        cycle++;

        pthread_mutex_lock(&grid->lock);

        // total served energy
        int total_served =
            grid->region_served[0] +
            grid->region_served[1] +
            grid->region_served[2];

        // total demand = served + deficits
        int total_demand =
            total_served +
            grid->region_deficit[0] +
            grid->region_deficit[1] +
            grid->region_deficit[2];

        // allocation efficiency
        float allocation_rate;

        if (total_demand > 0) {
            allocation_rate =
                ((float) total_served / total_demand) * 100;
        }
        else {
            allocation_rate = 100.0f;
        }

        // fairness calculation
        int min_served = grid->region_served[0];
        int max_served = grid->region_served[0];

        for (int i = 1; i < NUM_REGIONS; i++) {

            if (grid->region_served[i] < min_served) {
                min_served = grid->region_served[i];
            }

            if (grid->region_served[i] > max_served) {
                max_served = grid->region_served[i];
            }
        }

        float fairness;

        if (max_served > 0) {
            fairness = (float) min_served / max_served;
        }
        else {
            fairness = 1.0f;
        }

        printf("\n");
        printf("========================================\n");
        printf("         GRID METRICS REPORT\n");
        printf("========================================\n");

        printf("Cycle Number       : %d\n", cycle);

        printf("Allocation Rate    : %.2f%%\n",
               allocation_rate);

        printf("Fairness Index     : %.2f\n",
               fairness);

        printf("Current Grid Load  : %d/%d\n",
               grid->current_load,
               MAX_CAPACITY);

        printf("\n");

        printf("Region Statistics\n");

        printf("Residential Served : %d\n",
               grid->region_served[0]);

        printf("Industrial Served  : %d\n",
               grid->region_served[1]);

        printf("Commercial Served  : %d\n",
               grid->region_served[2]);

        printf("\n");

        printf("Region Deficits\n");

        printf("Residential Deficit : %d\n",
               grid->region_deficit[0]);

        printf("Industrial Deficit  : %d\n",
               grid->region_deficit[1]);

        printf("Commercial Deficit  : %d\n",
               grid->region_deficit[2]);

        printf("========================================\n\n");

        fflush(stdout);

        pthread_mutex_unlock(&grid->lock);
    }

    return NULL;
}
