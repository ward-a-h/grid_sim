#include "grid.h"

void balance_load(GridState* grid) {

    // Mutex is already locked before calling this function

    int available = grid->current_load;

    // Default priority order
    int order[NUM_REGIONS] = {0, 1, 2};

    // Find most deprived region
    int most_deprived = 0;

    if (grid->region_deficit[1] >
        grid->region_deficit[most_deprived]) {

        most_deprived = 1;
    }

    if (grid->region_deficit[2] >
        grid->region_deficit[most_deprived]) {

        most_deprived = 2;
    }

    // Dynamic fairness boost
    if (most_deprived != 0 &&
        grid->region_deficit[most_deprived] > 200) {

        order[1] = most_deprived;

        order[2] = (most_deprived == 1) ? 2 : 1;

        printf(YELLOW
               "[BALANCER] Fairness boost activated for Region %d\n"
               RESET,
               most_deprived);

        fflush(stdout);
    }

    // Serve regions
    for (int i = 0; i < NUM_REGIONS; i++) {

        int region = order[i];

        int demand = grid->region_demand[region];

        if (demand <= 0)
            continue;

        // Full allocation
        if (available >= demand) {

            available -= demand;

            grid->region_served[region] += demand;

            grid->region_deficit[region] = 0;

            printf(GREEN
                   "[BALANCER] Region %d fully served: %d units\n"
                   RESET,
                   region,
                   demand);
        }

        // Partial allocation
        else if (available > 0) {

            grid->region_served[region] += available;

            grid->region_deficit[region] +=
                (demand - available);

            printf(YELLOW
                   "[BALANCER] Region %d partially served: %d/%d | Deficit=%d\n"
                   RESET,
                   region,
                   available,
                   demand,
                   grid->region_deficit[region]);

            available = 0;
        }

        // No allocation
        else {

            grid->region_deficit[region] += demand;

            printf(RED
                   "[BALANCER] Region %d not served | Deficit=%d\n"
                   RESET,
                   region,
                   grid->region_deficit[region]);
        }

        fflush(stdout);
    }

    grid->current_load = available;
}
