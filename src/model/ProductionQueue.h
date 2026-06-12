#pragma once
#include <string>

class ProductionQueue {
public:
    void enqueue(const std::string& orderId, const std::string& sampleId,
                 int shortfall, double avgProductionTime, double yield);
};
