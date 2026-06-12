#pragma once
#include <string>
#include <chrono>

struct ProductionItem {
    std::string orderId;
    std::string sampleId;
    int actualQty = 0;
    std::chrono::steady_clock::time_point startedAt;
    double estimatedMinutes = 0.0;
};

struct QueueItem {
    std::string orderId;
    std::string sampleId;
    int shortfall = 0;
    int actualQty = 0;
    double estimatedMinutes = 0.0;
};
