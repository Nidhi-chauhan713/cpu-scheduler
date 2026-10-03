#ifndef SCHEDULER_ENGINE_H
#define SCHEDULER_ENGINE_H

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "process.h"
#include "scheduler.h"
#include "event_logger.h"

class SchedulerEngine {
public:
    const EventLogger& getLogger() const { return logger_; }

private:
    Scheduler* scheduler_;
    EventLogger logger_;

    // Concurrency primitives
    std::mutex engine_mutex_;
    std::condition_variable cv_engine_;   // Engine waits on this
    std::condition_variable cv_process_;  // Process threads wait on this

    // Execution state
    int current_time_{0};
    int active_pid_{-1};                  // PID of currently scheduled thread (-1 = idle)
    bool step_completed_{false};
    bool simulation_finished_{false};

    // Worker thread function
    void processWorker(Process* p, int quantum);

public:
    explicit SchedulerEngine(Scheduler* sched) : scheduler_(sched) {}
    ~SchedulerEngine() = default;
    
    void run(std::vector<Process>& processes, int quantum = 1);
};

#endif // SCHEDULER_ENGINE_H