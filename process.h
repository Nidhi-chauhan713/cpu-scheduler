#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>

struct Process {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;

    // Derived / tracking fields
    int remaining_time;
    int start_time{-1};      // -1 indicates process hasn't started yet
    int completion_time{0};
    int waiting_time{0};
    int turnaround_time{0};

    Process(int id, int arrival, int burst, int prio = 0)
        : pid(id),
          arrival_time(arrival),
          burst_time(burst),
          priority(prio),
          remaining_time(burst) {}

    // Resets tracking metrics (useful if re-running same processes on different algorithms)
    void reset() {
        remaining_time = burst_time;
        start_time = -1;
        completion_time = 0;
        waiting_time = 0;
        turnaround_time = 0;
    }
};

#endif // PROCESS_H