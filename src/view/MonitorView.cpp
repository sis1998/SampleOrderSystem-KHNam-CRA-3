#include "MonitorView.h"

void MonitorView::renderOrderCounts(std::ostream& os, const std::map<OrderStatus, int>& counts) {
    os << "=== Order Counts ===\n";
    auto print = [&](OrderStatus status, const char* label) {
        auto it = counts.find(status);
        int count = (it != counts.end()) ? it->second : 0;
        os << label << ": " << count << "\n";
    };
    print(OrderStatus::RESERVED,  "RESERVED");
    print(OrderStatus::PRODUCING, "PRODUCING");
    print(OrderStatus::CONFIRMED, "CONFIRMED");
    print(OrderStatus::RELEASE,   "RELEASE");
}

void MonitorView::renderStockStatus(std::ostream& os, const std::vector<Sample>& samples, const std::vector<Order>& orders) {
    os << "=== Stock Status ===\n";
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
           << " | Stock: " << s.stock
           << " | Status: " << state << "\n";
    }
}
