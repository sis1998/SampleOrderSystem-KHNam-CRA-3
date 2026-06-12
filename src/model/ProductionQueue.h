#pragma once
#include <deque>
#include <optional>
#include <string>
#include "../domain/Production.h"
#include "SampleModel.h"
#include "OrderModel.h"

class ProductionQueue {
public:
    void enqueue(const std::string& orderId, const std::string& sampleId,
                 int shortfall, double avgProductionTime, double yield);
    bool tick(SampleModel& sm, OrderModel& om);
    std::optional<ProductionItem> getCurrent() const;
    std::deque<QueueItem> getWaiting() const;
    int totalQueueSize() const;
    void restore(std::optional<ProductionItem> current, std::deque<QueueItem> waiting);
private:
    std::optional<ProductionItem> current_;
    std::deque<QueueItem> waiting_;
};
