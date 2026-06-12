#pragma once
#include <optional>
#include <string>
#include <vector>
#include "../domain/Order.h"

class OrderModel {
public:
    std::string create(const std::string& sampleId,
                       const std::string& customerName,
                       int quantity);
    std::optional<Order> findById(const std::string& orderId) const;
    std::vector<Order> getByStatus(OrderStatus status) const;
    bool transitionTo(const std::string& orderId, OrderStatus newStatus);
    void markStockDeducted(const std::string& orderId);
    void restore(std::vector<Order> orders, int nextId);
    std::vector<Order> getAll() const;
    int getNextId() const;
private:
    std::vector<Order> orders_;
    int nextId_ = 1;
};
