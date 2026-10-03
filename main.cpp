#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <map>
#include <string>
#include "process.h"
#include "fcfs_scheduler.h"
#include "round_robin_scheduler.h"
#include "priority_scheduler.h"
#include "scheduler_engine.h"
#include "event_logger.h"

void printGanttChart(const std::vector<Event>& logs, const std::string& scheduler_name) {
    std::cout << "\n========================================\n";
    std::cout << "   EVENT LOG: " << scheduler_name << "\n";
    std::cout << "========================================\n";
    std::cout << std::setw(10) << "Time" 
              << std::setw(10) << "PID" 
              << std::setw(15) << "Action" << std::endl;
    std::cout << "----------------------------------------\n";

    for (const auto& event : logs) {
        std::cout << std::setw(10) << event.timestamp;
        if (event.pid == -1) {
            std::cout << std::setw(10) << "IDLE";
        } else {
            std::cout << std::setw(10) << event.pid;
        }
        std::cout << std::setw(15) << event.action << std::endl;
    }
    std::cout << "========================================\n\n" << std::flush;
}

// Renders a compact visual Gantt bar (e.g. |--P1--|--P2--|) from the event log.
// Only STARTED / RESUMED / IDLE events represent an actual 1-unit execution tick;
// PREEMPTED / FINISHED are just annotations logged on top of a tick that already happened.
void printGanttBar(const std::vector<Event>& logs) {
    std::map<int, int> tick_pid; // time -> pid running during [time, time+1), -1 = idle
    int max_time = 0;

    for (const auto& e : logs) {
        if (e.action == "STARTED" || e.action == "RESUMED" || e.action == "IDLE") {
            tick_pid[e.timestamp] = e.pid;
            max_time = std::max(max_time, e.timestamp + 1);
        }
    }
    if (tick_pid.empty()) {
        return;
    }

    // Merge consecutive ticks belonging to the same pid into single visual segments
    struct Segment { int start; int end; int pid; };
    std::vector<Segment> segments;
    for (int t = 0; t < max_time; ++t) {
        int pid = tick_pid.count(t) ? tick_pid[t] : -1;
        if (!segments.empty() && segments.back().pid == pid) {
            segments.back().end = t + 1;
        } else {
            segments.push_back({t, t + 1, pid});
        }
    }

    const int unit_width = 5; // characters of bar per single time unit
    std::cout << "Gantt Chart:\n";

    // Bar row with centered PID/IDLE labels
    for (const auto& s : segments) {
        int width = (s.end - s.start) * unit_width;
        std::string label = (s.pid == -1) ? "IDLE" : ("P" + std::to_string(s.pid));
        int pad = std::max(0, width - 1 - static_cast<int>(label.size()));
        int left = pad / 2;
        int right = pad - left;
        std::cout << "|" << std::string(left, ' ') << label << std::string(right, ' ');
    }
    std::cout << "|\n";

    // Timestamp row aligned under each segment boundary
    for (const auto& s : segments) {
        int width = (s.end - s.start) * unit_width;
        std::string t_str = std::to_string(s.start);
        std::cout << t_str << std::string(std::max(1, width - static_cast<int>(t_str.size())), ' ');
    }
    std::cout << max_time << "\n\n";
}

// Prints per-process completion/waiting/turnaround times plus averages.
void printMetricsTable(const std::vector<Process>& processes, const std::string& scheduler_name) {
    std::vector<Process> sorted_procs = processes;
    std::sort(sorted_procs.begin(), sorted_procs.end(), [](const Process& a, const Process& b) {
        return a.pid < b.pid;
    });

    std::cout << "Performance Metrics: " << scheduler_name << "\n";
    std::cout << std::setw(6) << "PID" << std::setw(10) << "Arrival" << std::setw(8) << "Burst"
              << std::setw(12) << "Completion" << std::setw(10) << "Waiting"
              << std::setw(12) << "Turnaround" << "\n";
    std::cout << std::string(58, '-') << "\n";

    double total_waiting = 0.0, total_turnaround = 0.0;
    for (const auto& p : sorted_procs) {
        std::cout << std::setw(6) << p.pid << std::setw(10) << p.arrival_time
                  << std::setw(8) << p.burst_time << std::setw(12) << p.completion_time
                  << std::setw(10) << p.waiting_time << std::setw(12) << p.turnaround_time << "\n";
        total_waiting += p.waiting_time;
        total_turnaround += p.turnaround_time;
    }

    std::cout << std::string(58, '-') << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Average Waiting Time:    " << (total_waiting / sorted_procs.size()) << "\n";
    std::cout << "Average Turnaround Time: " << (total_turnaround / sorted_procs.size()) << "\n\n";
    std::cout.unsetf(std::ios::fixed);
}

int main() {
    // Enable auto-flushing on std::cout
    std::cout << std::unitbuf;

    std::cout << "========================================\n";
    std::cout << "   CPU Scheduler Simulator - Phase 3    \n";
    std::cout << "========================================\n" << std::endl;

    std::vector<Process> processes = {
        Process(1, 0, 5, 2),
        Process(2, 1, 3, 1),
        Process(3, 2, 4, 3)
    };

    std::cout << "[1] Executing FCFS Scheduler..." << std::endl;
    FCFSScheduler fcfs;
    SchedulerEngine engine1(&fcfs);

    auto fcfs_procs = processes;
    engine1.run(fcfs_procs, 0);
    printGanttChart(engine1.getLogger().getLogs(), "FCFS");
    printGanttBar(engine1.getLogger().getLogs());
    printMetricsTable(fcfs_procs, "FCFS");

    std::cout << "[2] Executing Round Robin Scheduler (Quantum = 2)..." << std::endl;
    RoundRobinScheduler rr(2);
    SchedulerEngine engine2(&rr);

    auto rr_procs = processes;
    engine2.run(rr_procs, 2);
    printGanttChart(engine2.getLogger().getLogs(), "Round Robin (q=2)");
    printGanttBar(engine2.getLogger().getLogs());
    printMetricsTable(rr_procs, "Round Robin (q=2)");

    std::cout << "[3] Executing Priority Scheduler..." << std::endl;
    PriorityScheduler pr;
    SchedulerEngine engine3(&pr);

    auto pr_procs = processes;
    engine3.run(pr_procs, 0);
    printGanttChart(engine3.getLogger().getLogs(), "Priority");
    printGanttBar(engine3.getLogger().getLogs());
    printMetricsTable(pr_procs, "Priority");

    std::cout << "All scheduling simulations completed successfully!" << std::endl;
    return 0;
}