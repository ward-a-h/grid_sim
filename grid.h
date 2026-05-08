#ifndef GRID_H //basically means if it isn't defined yet, proceed. Otherwise, skip. 
#define GRID_H

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_CAPACITY 1000
#define NUM_GENERATORS 3 //3 power plants: Coal, Wind, Solar
#define NUM_REGIONS 3 //3 consumer areas: Residential, Commercial, Industrial

typedef struct {   //manahil
    int total_demanded;
    int total_served;
    int fault_count;
    int recovery_count;
} Metrics;
typedef struct {
    int current_load; //total energy currently in the grid
    int region_demand[NUM_REGIONS]; //what each region is demanding
    int region_served[NUM_REGIONS]; //what each region is consuming
    int region_deficit[NUM_REGIONS]; //how much each region has been denied over time(deficit)
    int generator_active[NUM_GENERATORS];//1=ON (generator runs normally)
    //0=OFF (generator broke down due to a fault)
    Metrics metrics;
    pthread_mutex_t lock; //mutex (only 1 thread can access data at a point)
    sem_t capacity_sem;//counting semaphore(tracks count of how much resource available)
    pthread_cond_t demand_change;//signals all sleeping threads when a change occurs
} GridState;

// Generator thread functions (Wardah)
void* coal_generator(void* arg);
void* solar_generator(void* arg);
void* wind_generator(void* arg);

// Consumer thread functions (Ayesha)
void* residential_consumer(void* arg);
void* industrial_consumer(void* arg);
void* commercial_consumer(void* arg);

// Load balancer (Manahil)
void balance_load(GridState* grid);
void* fault_thread(void* arg);
void* metrics_thread(void* arg);

#endif
