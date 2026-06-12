#pragma once

#include <string>
#include <chrono>

namespace domain {

// Item currently being produced
struct ProductionItem {
    std::string                              orderId;
    std::string                              sampleId;
    int                                      actualQty;
    std::chrono::steady_clock::time_point    startedAt;
    double                                   estimatedMinutes;
};

// Item waiting in the production queue
struct QueueItem {
    std::string orderId;
    std::string sampleId;
    int         shortfall;        // units still needed to fulfil the order
    int         actualQty;
    double      estimatedMinutes;
};

} // namespace domain
