#include "fcfs_scheduler.h"

void FCFSScheduler::addProcess(Process* p) {
    ready_queue_.push(p);
}

Process* FCFSScheduler::selectNextProcess([[maybe_unused]] int current_time) {
    if (ready_queue_.empty()) {
        return nullptr;
    }
    
    Process* p = ready_queue_.front();
    ready_queue_.pop();
    return p;
}

bool FCFSScheduler::empty() const {
    return ready_queue_.empty();
}