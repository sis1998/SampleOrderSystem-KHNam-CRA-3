#include "OrderView.h"

static void renderOrderList(std::ostream& os, std::string_view header,
                            const std::vector<Order>& orders) {
    os << header << "\n";
    for (int i = 0; i < static_cast<int>(orders.size()); ++i) {
        const auto& o = orders[i];
        os << " " << (i + 1) << ". [" << o.orderId << "] " << o.sampleId
           << " | 고객: " << o.customerName << " | 수량: " << o.quantity << "\n";
    }
}

void OrderView::renderReservedList(std::ostream& os, const std::vector<Order>& orders) {
    renderOrderList(os, "=== 승인 대기 주문 ===", orders);
}

void OrderView::renderConfirmedList(std::ostream& os, const std::vector<Order>& orders) {
    renderOrderList(os, "=== 출고 대기 주문 ===", orders);
}

void OrderView::renderMessage(std::ostream& os, std::string_view message) {
    os << message << "\n";
}
