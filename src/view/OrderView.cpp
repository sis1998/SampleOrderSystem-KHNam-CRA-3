#include "OrderView.h"

void OrderView::renderReservedList(std::ostream& os, const std::vector<Order>& orders) {
    os << "=== Reserved Orders ===\n";
    for (const auto& o : orders) {
        os << "[" << o.orderId << "] " << o.sampleId
           << " | Customer: " << o.customerName
           << " | Qty: " << o.quantity << "\n";
    }
}

void OrderView::renderConfirmedList(std::ostream& os, const std::vector<Order>& orders) {
    os << "=== Confirmed Orders ===\n";
    for (const auto& o : orders) {
        os << "[" << o.orderId << "] " << o.sampleId
           << " | Customer: " << o.customerName
           << " | Qty: " << o.quantity << "\n";
    }
}

void OrderView::renderMessage(std::ostream& os, std::string_view message) {
    os << message << "\n";
}
