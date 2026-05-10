#include "grid.h" //brings everything from the header file, no <> used as its a local file

int main() {
    GridState grid; //every threads reads and writes to this single object
    printf(BOLD "╔══════════════════════════════════════════╗\n" RESET);
    printf(BOLD "║     Smart Energy Grid Simulation         ║\n" RESET);
    printf(BOLD "║     Max Capacity: 1000 units             ║\n" RESET);
    printf(BOLD "║     Generators: Coal, Solar, Wind        ║\n" RESET);
    printf(BOLD "║     Regions: Residential, Industrial,    ║\n" RESET);
    printf(BOLD "║              Commercial                  ║\n" RESET);
    printf(BOLD "╚══════════════════════════════════════════╝\n" RESET);
    printf("\n");
    // Initialize all fields to zero, grid starts empty
    grid.current_load = 0;
    for (int i = 0; i < NUM_REGIONS; i++) { //Clean state. No demand, no serve, no deficit.
        grid.region_demand[i] = 0;
        grid.region_served[i] = 0;
        grid.region_deficit[i] = 0;
    }
    grid.metrics.total_demanded = 0;
    grid.metrics.total_served = 0;
    grid.metrics.fault_count = 0;
    grid.metrics.recovery_count = 0;
    grid.stop = 0; //simulation runs until metrics thread sets this to 1
    grid.tick = 0; //simulation starts at tick 0
    for (int i = 0; i < NUM_GENERATORS; i++) {
        grid.generator_active[i] = 1; // all start as active
    }

    // Initialize moutex, semaphore and the condition variable
    pthread_mutex_init(&grid.lock, NULL);
    sem_init(&grid.capacity_sem, 0, MAX_CAPACITY);
    //0 means its shared between threads of the same process
    pthread_cond_init(&grid.demand_change, NULL);

    // Separate thread ids (3 for generators, 3 for consumers, 1 for fault)
    pthread_t gen_threads[3], con_threads[3], fault_t;

    // Creating generator threads
    pthread_create(&gen_threads[0], NULL, coal_generator, &grid);
    pthread_create(&gen_threads[1], NULL, solar_generator, &grid);
    pthread_create(&gen_threads[2], NULL, wind_generator, &grid);

    // Creating consumer threads (Ayesha's)
    pthread_create(&con_threads[0], NULL, residential_consumer, &grid);
    pthread_create(&con_threads[1], NULL, industrial_consumer, &grid);
    pthread_create(&con_threads[2], NULL, commercial_consumer, &grid);

    // Creating fault thread (Manahil's)
    pthread_create(&fault_t, NULL, fault_thread, &grid);
    pthread_t metrics_t;
    pthread_t clock_t;
    pthread_create(&metrics_t, NULL, metrics_thread, &grid);
    pthread_create(&clock_t, NULL, clock_thread, &grid);

    // pthread_join tells main to wait for a thread to finish before continuing 
    for (int i = 0; i < 3; i++) {
    pthread_join(gen_threads[i], NULL);
    }
    for (int i = 0; i < 3; i++) {
    pthread_join(con_threads[i], NULL);
    }
    pthread_join(fault_t, NULL);
    pthread_join(metrics_t, NULL);
    pthread_join(clock_t, NULL);
    //Free the memory used by mutex, semaphore and condition variable 
    pthread_mutex_destroy(&grid.lock);
    sem_destroy(&grid.capacity_sem);
    pthread_cond_destroy(&grid.demand_change);

    return 0;
}
