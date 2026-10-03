#ifndef FCFS_SCHEDULER_H
#define FCFS_SCHEDULER_H

#include "scheduler.h"
#include <queue>

class FCFSScheduler : public Scheduler {
private:
    std::queue<Process*> ready_queue_;

public:
    void addProcess(Process* p) override;
    Process* selectNextProcess(int current_time) override;
    bool empty() const override;
};

#endif // FCFS_SCHEDULER_H