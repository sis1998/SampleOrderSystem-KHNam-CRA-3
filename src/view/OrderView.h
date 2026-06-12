#pragma once
#include <ostream>
#include <string_view>
#include <vector>
#include "../domain/Order.h"

class OrderView {
public:
    void renderReservedList(std::ostream& os, const std::vector<Order>& orders);
    void renderConfirmedList(std::ostream& os, const std::vector<Order>& orders);
    void renderMessage(std::ostream& os, std::string_view message);
};
