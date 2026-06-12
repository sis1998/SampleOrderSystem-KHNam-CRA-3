#include "ProductionView.h"

void ProductionView::renderQueue(std::ostream& os, const std::optional<ProductionItem>& current, const std::deque<QueueItem>& queue) {
    os << "=== 생산 대기열 ===\n";
    if (current.has_value()) {
        os << "[진행중] 주문: " << current->orderId
           << " | 시료: " << current->sampleId
           << " | 수량: " << current->actualQty
           << " | 예상: " << current->estimatedMinutes << " 분\n";
    } else {
        os << "[진행중] 없음\n";
    }
    os << "--- 대기 ---\n";
    for (const auto& item : queue) {
        os << "주문: " << item.orderId
           << " | 시료: " << item.sampleId
           << " | 수량: " << item.actualQty
           << " | 예상: " << item.estimatedMinutes << " 분\n";
    }
}
