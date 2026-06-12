#pragma once
#include <ostream>
#include <map>
#include <vector>
#include "../domain/Order.h"
#include "../domain/Sample.h"

class MonitorView {
public:
    void renderOrderCounts(std::ostream& os, const std::map<OrderStatus, int>& counts);
    void renderStockStatus(std::ostream& os, const std::vector<Sample>& samples, const std::vector<Order>& orders);
};
