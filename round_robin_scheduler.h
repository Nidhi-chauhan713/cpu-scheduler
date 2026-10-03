#ifndef ROUND_ROBIN_SCHEDULER_H
#define ROUND_ROBIN_SCHEDULER_H

#include "scheduler.h"
#include <queue>

class RoundRobinScheduler : public Scheduler {
private:
    std::queue<Process*> ready_queue_;
    int time_quantum_;

public:
    explicit RoundRobinScheduler(int quantum) : time_quantum_(quantum) {}

    void addProcess(Process* p) override;
    Process* selectNextProcess(int current_time) override;
    bool empty() const override;

    int getTimeQuantum() const { return time_quantum_; }
};

#endif // ROUND_ROBIN_SCHEDULER_H