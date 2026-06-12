#pragma once
#include <string>

enum class OrderStatus { RESERVED, REJECTED, PRODUCING, CONFIRMED, RELEASE };

struct Order {
    std::string orderId;
    std::string sampleId;
    std::string customerName;
    int quantity = 0;
    OrderStatus status = OrderStatus::RESERVED;
    std::string createdAt;
    bool stockPreDeducted = false; // true when stock was deducted at approval
};
