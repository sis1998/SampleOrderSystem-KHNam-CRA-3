#include "ProductionQueue.h"
#include <cmath>
#include <chrono>

void ProductionQueue::enqueue(const std::string& orderId,
                               const std::string& sampleId,
                               int shortfall,
                               double avgProductionTime,
                               double yield) {
    int actualQty = static_cast<int>(std::ceil(shortfall / (yield * 0.9)));
    double estimatedMinutes = avgProductionTime * actualQty;

    QueueItem item;
    item.orderId = orderId;
    item.sampleId = sampleId;
    item.shortfall = shortfall;
    item.actualQty = actualQty;
    item.estimatedMinutes = estimatedMinutes;

    waiting_.push_back(item);
}

bool ProductionQueue::tick(SampleModel& sm, OrderModel& om) {
    if (!current_.has_value() && !waiting_.empty()) {
        QueueItem front = waiting_.front();
        waiting_.pop_front();

        ProductionItem item;
        item.orderId = front.orderId;
        item.sampleId = front.sampleId;
        item.actualQty = front.actualQty;
        item.estimatedMinutes = front.estimatedMinutes;
        item.startedAt = std::chrono::steady_clock::now();

        current_ = item;
        return false;
    }

    if (current_.has_value()) {
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double, std::ratio<60>>(
            now - current_->startedAt).count();

        if (elapsed >= current_->estimatedMinutes) {
            sm.addStock(current_->sampleId, current_->actualQty);
            om.transitionTo(current_->orderId, OrderStatus::CONFIRMED);
            current_.reset();
            return true;
        }
    }

    return false;
}

std::optional<ProductionItem> ProductionQueue::getCurrent() const {
    return current_;
}

std::deque<QueueItem> ProductionQueue::getWaiting() const {
    return waiting_;
}
