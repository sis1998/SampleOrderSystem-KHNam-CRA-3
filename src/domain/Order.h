#pragma once

#include <string>

namespace domain {

enum class OrderStatus {
    RESERVED,
    REJECTED,
    PRODUCING,
    CONFIRMED,
    RELEASE
};

struct Order {
    std::string orderId;      // e.g. "ORD-1"
    std::string sampleId;
    std::string customerName;
    int         quantity;     // >= 1
    OrderStatus status;
    std::string createdAt;    // ISO 8601 string
};

} // namespace domain
