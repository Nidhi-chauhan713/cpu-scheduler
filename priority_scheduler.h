#ifndef PRIORITY_SCHEDULER_H
#define PRIORITY_SCHEDULER_H

#include "scheduler.h"
#include <queue>

// Comparator struct: lower priority value = higher execution priority
struct ProcessPriorityComparator {
    bool operator()(const Process* a, const Process* b) const {
        if (a->priority == b->priority) {
            return a->arrival_time > b->arrival_time; // FCFS tie-breaker
        }
        return a->priority > b->priority;
    }
};

class PriorityScheduler : public Scheduler {
private:
    std::priority_queue<Process*, std::vector<Process*>, ProcessPriorityComparator> ready_queue_;

public:
    void addProcess(Process* p) override;
    Process* selectNextProcess(int current_time) override;
    bool empty() const override;
};

#endif // PRIORITY_SCHEDULER_H