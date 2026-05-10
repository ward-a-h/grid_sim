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

        printf(BOLD CYAN);
        printf("╔════════════════ SMART ENERGY GRID ════════════════╗\n");
        printf(RESET);

        printf("║ Cycle Number      : %-28d ║\n",
               cycle);

        printf("║ Current Grid Load : %-6d / %-15d ║\n",
               grid->current_load,
               MAX_CAPACITY);

        // allocation rate color
        if (allocation_rate >= 80)
            printf(GREEN);
        else if (allocation_rate >= 50)
            printf(YELLOW);
        else
            printf(RED);

        printf("║ Allocation Rate   : %-27.2f%% ║\n",
               allocation_rate);

        printf(RESET);

        // fairness color
        if (fairness >= 0.8)
            printf(GREEN);
        else if (fairness >= 0.5)
            printf(YELLOW);
        else
            printf(RED);

        printf("║ Fairness Index    : %-28.2f ║\n",
               fairness);

        printf(RESET);

        printf(CYAN);
        printf("╠══════════════ REGION STATUS ══════════════════════╣\n");
        printf(RESET);

        printf("║ Residential Served : %-25d ║\n",
               grid->region_served[0]);

        printf("║ Industrial Served  : %-25d ║\n",
               grid->region_served[1]);

        printf("║ Commercial Served  : %-25d ║\n",
               grid->region_served[2]);

        printf(CYAN);
        printf("╠══════════════ REGION DEFICITS ════════════════════╣\n");
        printf(RESET);

        printf("%s║ Residential Deficit : %-24d ║%s\n",
               grid->region_deficit[0] > 0 ? RED : GREEN,
               grid->region_deficit[0],
               RESET);

        printf("%s║ Industrial Deficit  : %-24d ║%s\n",
               grid->region_deficit[1] > 0 ? RED : GREEN,
               grid->region_deficit[1],
               RESET);

        printf("%s║ Commercial Deficit  : %-24d ║%s\n",
               grid->region_deficit[2] > 0 ? RED : GREEN,
               grid->region_deficit[2],
               RESET);

        printf(CYAN);
        printf("╠══════════════ GENERATOR STATUS ═══════════════════╣\n");
        printf(RESET);

        printf("║ Coal Generator   : %s%-29s%s ║\n",
               grid->generator_active[0] ? GREEN : RED,
               grid->generator_active[0] ? "ONLINE" : "OFFLINE",
               RESET);

        printf("║ Solar Generator  : %s%-29s%s ║\n",
               grid->generator_active[1] ? GREEN : RED,
               grid->generator_active[1] ? "ONLINE" : "OFFLINE",
               RESET);

        printf("║ Wind Generator   : %s%-29s%s ║\n",
               grid->generator_active[2] ? GREEN : RED,
               grid->generator_active[2] ? "ONLINE" : "OFFLINE",
               RESET);

        printf(CYAN);
        printf("╠══════════════ SYSTEM EVENTS ══════════════════════╣\n");
        printf(RESET);

        printf("║ Fault Count      : %-28d ║\n",
               grid->metrics.fault_count);

        printf("║ Recovery Count   : %-28d ║\n",
               grid->metrics.recovery_count);

        printf(CYAN);
        printf("╚═══════════════════════════════════════════════════╝\n");
        printf(RESET);

        printf("\n");

        fflush(stdout);

        pthread_mutex_unlock(&grid->lock);
    }

    grid->stop = 1;

    printf(RED
           "\nSimulation complete. Shutting down all threads...\n"
           RESET);

    fflush(stdout);

    exit(0);

    return NULL;
}
