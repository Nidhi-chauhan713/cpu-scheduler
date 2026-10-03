#include "priority_scheduler.h"

void PriorityScheduler::addProcess(Process* p) {
    ready_queue_.push(p);
}

Process* PriorityScheduler::selectNextProcess([[maybe_unused]] int current_time) {
    if (ready_queue_.empty()) {
        return nullptr;
    }

    Process* p = ready_queue_.top();
    ready_queue_.pop();
    return p;
}

bool PriorityScheduler::empty() const {
    return ready_queue_.empty();
}