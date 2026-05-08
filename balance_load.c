#include "grid.h"

void balance_load(GridState* grid) {
    // Mutex is already locked before calling this function

    int available = grid->current_load;

    // Default priority order
    int order[NUM_REGIONS] = {0, 1, 2};

    
    if (grid->region_deficit[2] > 500) {
        order[1] = 2;
        order[2] = 1;
       printf("[BALANCER] Fairness boost activated for Commercial region\n");
        fflush(stdout);
    }

    for (int i = 0; i < NUM_REGIONS; i++) {
        int region = order[i];
        int demand = grid->region_demand[region];

        if (demand <= 0)
            continue;

        if (available >= demand) {
            // Full allocation
            available -= demand;

            grid->region_served[region] += demand;
            grid->region_deficit[region] = 0;

            printf("[BALANCER] Region %d fully served: %d units\n",
                   region, demand);
        }
        else if (available > 0) {
            // Partial allocation
            grid->region_served[region] += available;
            grid->region_deficit[region] += (demand - available);

            printf("[BALANCER] Region %d partially served: %d/%d | Deficit=%d\n",
                   region,
                   available,
                   demand,
                   grid->region_deficit[region]);

            available = 0;
        }
        else {
            // No allocation
            grid->region_deficit[region] += demand;

            printf("[BALANCER] Region %d not served | Deficit=%d\n",
                   region,
                   grid->region_deficit[region]);
        }

        fflush(stdout);
    }

    grid->current_load = available;
}
