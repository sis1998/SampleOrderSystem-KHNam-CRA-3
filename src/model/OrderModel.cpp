#include "OrderModel.h"
#include <ctime>
#include <algorithm>

static std::string currentIso8601() {
    std::time_t t = std::time(nullptr);
    std::tm tm_buf{};
    localtime_s(&tm_buf, &t);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm_buf);
    return buf;
}

static bool isValidTransition(OrderStatus from, OrderStatus to) {
    if (from == OrderStatus::RESERVED) {
        return to == OrderStatus::CONFIRMED ||
               to == OrderStatus::PRODUCING ||
               to == OrderStatus::REJECTED;
    }
    if (from == OrderStatus::PRODUCING) {
        return to == OrderStatus::CONFIRMED;
    }
    if (from == OrderStatus::CONFIRMED) {
        return to == OrderStatus::RELEASE;
    }
    // REJECTED and RELEASE are terminal
    return false;
}

std::string OrderModel::create(const std::string& sampleId,
                                const std::string& customerName,
                                int quantity) {
    Order o;
    o.orderId = "ORD-" + std::to_string(nextId_++);
    o.sampleId = sampleId;
    o.customerName = customerName;
    o.quantity = quantity;
    o.status = OrderStatus::RESERVED;
    o.createdAt = currentIso8601();
    orders_.push_back(o);
    return o.orderId;
}

std::optional<Order> OrderModel::findById(const std::string& orderId) const {
    for (const auto& o : orders_) {
        if (o.orderId == orderId) return o;
    }
    return std::nullopt;
}

std::vector<Order> OrderModel::getByStatus(OrderStatus status) const {
    std::vector<Order> result;
    for (const auto& o : orders_) {
        if (o.status == status) result.push_back(o);
    }
    return result;
}

bool OrderModel::transitionTo(const std::string& orderId, OrderStatus newStatus) {
    for (auto& o : orders_) {
        if (o.orderId == orderId) {
            if (!isValidTransition(o.status, newStatus)) return false;
            o.status = newStatus;
            return true;
        }
    }
    return false;
}
