#include "ProductionView.h"

void ProductionView::renderQueue(std::ostream& os, const std::optional<ProductionItem>& current, const std::deque<QueueItem>& queue) {
    os << "=== Production Queue ===\n";
    if (current.has_value()) {
        os << "[Active] Order: " << current->orderId
           << " | Sample: " << current->sampleId
           << " | Qty: " << current->actualQty
           << " | ETA: " << current->estimatedMinutes << " min\n";
    } else {
        os << "[Active] none\n";
    }
    os << "--- Waiting ---\n";
    for (const auto& item : queue) {
        os << "Order: " << item.orderId
           << " | Sample: " << item.sampleId
           << " | Qty: " << item.actualQty
           << " | ETA: " << item.estimatedMinutes << " min\n";
    }
}
