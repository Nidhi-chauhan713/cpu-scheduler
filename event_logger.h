#ifndef EVENT_LOGGER_H
#define EVENT_LOGGER_H

#include <iostream>
#include <string>
#include <vector>
#include <mutex>

struct Event {
    int timestamp;
    int pid; // -1 represents CPU IDLE
    std::string action; // "STARTED", "RUNNING", "PREEMPTED", "FINISHED"
};

class EventLogger {
private:
    std::vector<Event> log_;
    mutable std::mutex log_mutex_;

public:
    void logEvent(int time, int pid, const std::string& action);
    std::vector<Event> getLogs() const;
    void clear();
};

#endif // EVENT_LOGGER_H