#include "JsonSerializer.h"
#include <chrono>
#include <string>

// OrderStatus <-> string mapping
static std::string statusToString(OrderStatus s) {
    switch (s) {
    case OrderStatus::RESERVED:  return "RESERVED";
    case OrderStatus::REJECTED:  return "REJECTED";
    case OrderStatus::PRODUCING: return "PRODUCING";
    case OrderStatus::CONFIRMED: return "CONFIRMED";
    case OrderStatus::RELEASE:   return "RELEASE";
    }
    return "RESERVED";
}

static OrderStatus statusFromString(const std::string& s) {
    if (s == "REJECTED")  return OrderStatus::REJECTED;
    if (s == "PRODUCING") return OrderStatus::PRODUCING;
    if (s == "CONFIRMED") return OrderStatus::CONFIRMED;
    if (s == "RELEASE")   return OrderStatus::RELEASE;
    return OrderStatus::RESERVED;
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
    s.sampleId = j["sampleId"].get<std::string>();
    s.name = j["name"].get<std::string>();
    s.avgProductionTime = j["avgProductionTime"].get<double>();
    s.yield = j["yield"].get<double>();
    s.stock = j["stock"].get<int>();
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
    o.orderId = j["orderId"].get<std::string>();
    o.sampleId = j["sampleId"].get<std::string>();
    o.customerName = j["customerName"].get<std::string>();
    o.quantity = j["quantity"].get<int>();
    o.status = statusFromString(j["status"].get<std::string>());
    o.createdAt = j["createdAt"].get<std::string>();
    o.stockPreDeducted = j["stockPreDeducted"].get<bool>();
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
    q.orderId = j["orderId"].get<std::string>();
    q.sampleId = j["sampleId"].get<std::string>();
    q.shortfall = j["shortfall"].get<int>();
    q.actualQty = j["actualQty"].get<int>();
    q.estimatedMinutes = j["estimatedMinutes"].get<double>();
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
    item.orderId = j["orderId"].get<std::string>();
    item.sampleId = j["sampleId"].get<std::string>();
    item.actualQty = j["actualQty"].get<int>();
    item.estimatedMinutes = j["estimatedMinutes"].get<double>();
    double elapsed = j["elapsed_min"].get<double>();
    item.startedAt = std::chrono::steady_clock::now()
        - std::chrono::duration_cast<std::chrono::steady_clock::duration>(
            std::chrono::duration<double, std::ratio<60>>(elapsed));
    return item;
}
