#include "OrderView.h"

void OrderView::renderReservedList(std::ostream& os, const std::vector<Order>& orders) {
    os << "=== 승인 대기 주문 ===\n";
    for (const auto& o : orders) {
        os << "[" << o.orderId << "] " << o.sampleId
           << " | 고객: " << o.customerName
           << " | 수량: " << o.quantity << "\n";
    }
}

void OrderView::renderConfirmedList(std::ostream& os, const std::vector<Order>& orders) {
    os << "=== 출고 대기 주문 ===\n";
    for (const auto& o : orders) {
        os << "[" << o.orderId << "] " << o.sampleId
           << " | 고객: " << o.customerName
           << " | 수량: " << o.quantity << "\n";
    }
}

void OrderView::renderMessage(std::ostream& os, std::string_view message) {
    os << message << "\n";
}
