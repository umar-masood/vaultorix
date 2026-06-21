#pragma once

#include <atomic>
#include <thread>

class TaskManager {
    public:
    static std::atomic_bool shutdownRequested;
};

