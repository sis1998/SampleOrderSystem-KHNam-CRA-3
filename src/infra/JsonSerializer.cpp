#include "JsonSerializer.h"
#include <chrono>
#include <stdexcept>
#include <string>

// OrderStatus <-> string mapping
static std::string statusToString(OrderStatus s) {
    switch (s) {
    case OrderStatus::RESERVED:  return "RESERVED";
    case OrderStatus::REJECTED:  return "REJECTED";
    case OrderStatus::PRODUCING: return "PRODUCING";
    case OrderStatus::CONFIRMED: return "CONFIRMED";
    case OrderStatus::RELEASE:   return "RELEASE";
    default: throw std::invalid_argument("Unknown OrderStatus value");
    }
}

static OrderStatus statusFromString(const std::string& s) {
    if (s == "RESERVED")  return OrderStatus::RESERVED;
    if (s == "REJECTED")  return OrderStatus::REJECTED;
    if (s == "PRODUCING") return OrderStatus::PRODUCING;
    if (s == "CONFIRMED") return OrderStatus::CONFIRMED;
    if (s == "RELEASE")   return OrderStatus::RELEASE;
    throw std::invalid_argument("Unknown OrderStatus: " + s);
}

// Sample
nlohmann::json toJson(const Sample& s) {
    return {
        {"sampleId", s.sampleId},
        {"name", s.name},
        {"avgProductionTime", s.avgProductionTime},
        {"yield", s.yield},
        {"stock", s.stock}
    };
}

Sample sampleFromJson(const nlohmann::json& j) {
    Sample s;
    s.sampleId = j.value("sampleId", std::string{});
    s.name = j.value("name", std::string{});
    s.avgProductionTime = j.value("avgProductionTime", 0.0);
    s.yield = j.value("yield", 0.0);
    s.stock = j.value("stock", 0);
    return s;
}

// Order
nlohmann::json toJson(const Order& o) {
    return {
        {"orderId", o.orderId},
        {"sampleId", o.sampleId},
        {"customerName", o.customerName},
        {"quantity", o.quantity},
        {"status", statusToString(o.status)},
        {"createdAt", o.createdAt},
        {"stockPreDeducted", o.stockPreDeducted}
    };
}

Order orderFromJson(const nlohmann::json& j) {
    Order o;
    o.orderId = j.value("orderId", std::string{});
    o.sampleId = j.value("sampleId", std::string{});
    o.customerName = j.value("customerName", std::string{});
    o.quantity = j.value("quantity", 0);
    o.status = statusFromString(j.value("status", std::string{"RESERVED"}));
    o.createdAt = j.value("createdAt", std::string{});
    o.stockPreDeducted = j.value("stockPreDeducted", false);
    return o;
}

// QueueItem
nlohmann::json toJson(const QueueItem& q) {
    return {
        {"orderId", q.orderId},
        {"sampleId", q.sampleId},
        {"shortfall", q.shortfall},
        {"actualQty", q.actualQty},
        {"estimatedMinutes", q.estimatedMinutes}
    };
}

QueueItem queueItemFromJson(const nlohmann::json& j) {
    QueueItem q;
    q.orderId = j.value("orderId", std::string{});
    q.sampleId = j.value("sampleId", std::string{});
    q.shortfall = j.value("shortfall", 0);
    q.actualQty = j.value("actualQty", 0);
    q.estimatedMinutes = j.value("estimatedMinutes", 0.0);
    return q;
}

// ProductionItem
nlohmann::json toJson(const ProductionItem& item) {
    double elapsed = std::chrono::duration<double, std::ratio<60>>(
        std::chrono::steady_clock::now() - item.startedAt).count();
    return {
        {"orderId", item.orderId},
        {"sampleId", item.sampleId},
        {"actualQty", item.actualQty},
        {"estimatedMinutes", item.estimatedMinutes},
        {"elapsed_min", elapsed}
    };
}

ProductionItem productionItemFromJson(const nlohmann::json& j) {
    ProductionItem item;
    item.orderId = j.value("orderId", std::string{});
    item.sampleId = j.value("sampleId", std::string{});
    item.actualQty = j.value("actualQty", 0);
    item.estimatedMinutes = j.value("estimatedMinutes", 0.0);
    double elapsed = j.value("elapsed_min", 0.0);
    item.startedAt = std::chrono::steady_clock::now()
        - std::chrono::duration_cast<std::chrono::steady_clock::duration>(
            std::chrono::duration<double, std::ratio<60>>(elapsed));
    return item;
}
