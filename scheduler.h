#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

class Scheduler {
public:
    virtual ~Scheduler() = default;

    // Adds a process to the ready queue when it arrives
    virtual void addProcess(Process* p) = 0;

    // Picks the next process to run based on current simulation time
    virtual Process* selectNextProcess(int current_time) = 0;

    // Returns true if no processes are waiting in the scheduler queue
    virtual bool empty() const = 0;
};

#endif // SCHEDULER_H