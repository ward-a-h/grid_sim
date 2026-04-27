# Smart Energy Grid Load Balancer – A Concurrency Simulation

A multithreaded energy grid simulation built in C using POSIX threads, mutexes, semaphores, and condition variables. Developed as an Operating Systems Theory project at FAST NUCES.

## What it does

Simulates a real-world energy grid where generator threads produce electricity and consumer threads demand it. The system handles shortages using priority-based load balancing, simulates random generator faults and recovery, and tracks fairness across regions.

## How to compile and run

Make sure you are on Linux or WSL with gcc installed. Then run:

make
./grid_sim

To clean the build:

make clean

## File structure

| File | Description |
|------|-------------|
| grid.h | Shared header — GridState struct, constants, and all function declarations |
| main.c | Entry point — initializes grid, spawns all threads, handles cleanup |
| generators.c | Coal, solar, and wind generator threads (producer side) |
| consumers.c | Residential, industrial, and commercial consumer threads (Ayesha) |
| load_balancer.c | Priority-based load balancing with fairness tracking (Manahil) |
| fault.c | Random fault injection and generator recovery thread (Manahil) |
| metrics.c | Periodic reporting of allocation rate and fairness index (Manahil) |
| Makefile | Compiles all files into the grid_sim executable |

## Team

| Name | Role |
|------|------|
| Wardah Ahmed (24K-1045) | Grid foundation, generator threads, main, Makefile |
| Ayesha Tariq (24K-0785) | Consumer threads |
| Manahil Malik (24K-0948) | Load balancer, fault thread, metrics |

## Course

Operating System Theory — FAST NUCES
Instructor: Sir Minhal


## System Architecture

```mermaid
flowchart TD
    subgraph GEN["Generator Threads - Producers"]
        Coal["Coal Generator\nConstant Output"]
        Solar["Solar Generator\nDaytime Only"]
        Wind["Wind Generator\nVariable Output"]
    end

    Fault["Fault Thread\nRandom Event Injection"]
    Grid["Grid Controller\nMutex-Protected Shared State\nSemaphore: 1000 unit limit\nCondition Variable: demand signals"]
    Balancer["Load Balancer\nPriority Allocation + Fairness Tracker"]

    subgraph CON["Consumer Threads - Regions"]
        Residential["Residential\nHigh Priority"]
        Industrial["Industrial\nMedium Priority"]
        Commercial["Commercial\nLow Priority"]
    end

    Monitor["Monitor Interface\nMetrics and Console Display"]

    Coal -- produce energy --> Grid
    Solar -- produce energy --> Grid
    Wind -- produce energy --> Grid
    Fault -- inject fault --> Grid
    Grid -- shortage signal --> Balancer
    Balancer -- allocate power --> Residential
    Balancer -- allocate power --> Industrial
    Balancer -- allocate power --> Commercial
    Residential -- demand and status --> Grid
    Industrial -- demand and status --> Grid
    Commercial -- demand and status --> Grid
    Residential --> Monitor
```
