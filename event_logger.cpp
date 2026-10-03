#include "event_logger.h"
#include <mutex>

void EventLogger::logEvent(int time, int pid, const std::string& action) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    log_.push_back({time, pid, action});
}

std::vector<Event> EventLogger::getLogs() const {
    std::lock_guard<std::mutex> lock(log_mutex_);
    return log_;
}

void EventLogger::clear() {
    std::lock_guard<std::mutex> lock(log_mutex_);
    log_.clear();
}