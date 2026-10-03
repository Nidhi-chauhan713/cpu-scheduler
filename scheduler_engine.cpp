#include "scheduler_engine.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

void SchedulerEngine::processWorker(Process* p, [[maybe_unused]] int quantum) {
    while (true) {
        std::unique_lock<std::mutex> lock(engine_mutex_);

        // Wait until it's either this process's turn to run OR simulation is done
        cv_process_.wait(lock, [this, p]() {
            return active_pid_ == p->pid || simulation_finished_;
        });

        if (simulation_finished_) {
            break;
        }

        // Record start time or resume event
        if (p->start_time == -1) {
            p->start_time = current_time_;
            logger_.logEvent(current_time_, p->pid, "STARTED");
        } else {
            logger_.logEvent(current_time_, p->pid, "RESUMED");
        }

        // Simulate 1 unit of CPU execution time
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        
        p->remaining_time--;
        
        if (p->remaining_time == 0) {
            p->completion_time = current_time_ + 1;
            p->turnaround_time = p->completion_time - p->arrival_time;
            p->waiting_time = p->turnaround_time - p->burst_time;
            logger_.logEvent(current_time_ + 1, p->pid, "FINISHED");
        }

        // Signal engine that 1 tick of work is done
        step_completed_ = true;
        active_pid_ = -1;
        cv_engine_.notify_one();

        if (p->remaining_time == 0) {
            break; // Worker thread terminates once process finishes burst
        }
    }
}

void SchedulerEngine::run(std::vector<Process>& processes, int quantum) {
    current_time_ = 0;
    simulation_finished_ = false;
    active_pid_ = -1;
    logger_.clear();

    // Reset process state
    for (auto& p : processes) {
        p.reset();
        // Defensive guard: a non-positive burst time would make remaining_time
        // skip past 0 in processWorker (0 -> -1 -> ...), so neither the worker
        // nor the dispatcher would ever see remaining_time == 0 and the
        // simulation would loop forever. Clamp to a minimum of 1 tick.
        if (p.remaining_time <= 0) {
            p.remaining_time = 1;
        }
    }

    // Ensure processes are sorted by arrival time
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });

    size_t total_processes = processes.size();
    size_t completed_count = 0;
    size_t next_arrival_idx = 0;

    // Spawn worker threads
    std::vector<std::thread> worker_threads;
    worker_threads.reserve(total_processes);
    for (auto& p : processes) {
        worker_threads.emplace_back(&SchedulerEngine::processWorker, this, &p, quantum);
    }

    Process* current_running = nullptr;
    int time_slice_used = 0;

    // Dispatcher Loop
    while (completed_count < total_processes) {
        std::unique_lock<std::mutex> lock(engine_mutex_);

        // 1. Add arrived processes
        while (next_arrival_idx < total_processes && processes[next_arrival_idx].arrival_time <= current_time_) {
            scheduler_->addProcess(&processes[next_arrival_idx]);
            next_arrival_idx++;
        }

        // 2. Pick process
        if (current_running == nullptr) {
            current_running = scheduler_->selectNextProcess(current_time_);
            time_slice_used = 0;
        }

        if (current_running == nullptr) {
            logger_.logEvent(current_time_, -1, "IDLE");
            current_time_++;
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            continue;
        }

        // 3. Trigger active worker thread
        active_pid_ = current_running->pid;
        step_completed_ = false;
        cv_process_.notify_all();

        // 4. Wait for worker tick completion
        cv_engine_.wait(lock, [this]() { return step_completed_; });

        // 5. Update tick state
        current_time_++;
        time_slice_used++;

        // 6. Check execution status
        if (current_running->remaining_time == 0) {
            completed_count++;
            current_running = nullptr;
            time_slice_used = 0;
        } else if (quantum > 0 && time_slice_used >= quantum) {
            logger_.logEvent(current_time_, current_running->pid, "PREEMPTED");
            
            while (next_arrival_idx < total_processes && processes[next_arrival_idx].arrival_time <= current_time_) {
                scheduler_->addProcess(&processes[next_arrival_idx]);
                next_arrival_idx++;
            }
            
            scheduler_->addProcess(current_running);
            current_running = nullptr;
            time_slice_used = 0;
        }
    }

    // Wrap up simulation and safely join worker threads
    {
        std::lock_guard<std::mutex> lock(engine_mutex_);
        simulation_finished_ = true;
        cv_process_.notify_all();
    }

    for (auto& th : worker_threads) {
        if (th.joinable()) {
            th.join();
        }
    }
}