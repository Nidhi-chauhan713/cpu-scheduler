#include "round_robin_scheduler.h"

void RoundRobinScheduler::addProcess(Process* p) {
    ready_queue_.push(p);
}

Process* RoundRobinScheduler::selectNextProcess([[maybe_unused]] int current_time) {
    if (ready_queue_.empty()) {
        return nullptr;
    }

    Process* p = ready_queue_.front();
    ready_queue_.pop();
    return p;
}

bool RoundRobinScheduler::empty() const {
    return ready_queue_.empty();
}