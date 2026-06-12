#include "MonitorView.h"

void MonitorView::renderOrderCounts(std::ostream& os, const std::map<OrderStatus, int>& counts) {
    os << "=== 주문 현황 ===\n";
    auto print = [&](OrderStatus status, const char* label) {
        auto it = counts.find(status);
        int count = (it != counts.end()) ? it->second : 0;
        os << label << ": " << count << "\n";
    };
    print(OrderStatus::RESERVED,  "접수");
    print(OrderStatus::PRODUCING, "생산중");
    print(OrderStatus::CONFIRMED, "출고대기");
    print(OrderStatus::RELEASE,   "출고완료");
}

void MonitorView::renderStockStatus(std::ostream& os, const std::vector<Sample>& samples, const std::vector<Order>& orders) {
    os << "=== 재고 현황 ===\n";
    for (const auto& s : samples) {
        int demand = 0;
        for (const auto& o : orders) {
            if (o.sampleId == s.sampleId) {
                if (o.status == OrderStatus::RESERVED ||
                    o.status == OrderStatus::PRODUCING ||
                    o.status == OrderStatus::CONFIRMED) {
                    demand += o.quantity;
                }
            }
        }
        const char* state;
        if (s.stock == 0) {
            state = "고갈";
        } else if (s.stock <= demand) {
            state = "부족";
        } else {
            state = "여유";
        }
        os << "[" << s.sampleId << "] " << s.name
           << " | 재고: " << s.stock
           << " | 상태: " << state << "\n";
    }
}
