#include "OrderRepository.h"
#include "../infra/JsonSerializer.h"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>

void OrderRepository::load(OrderModel& model, const std::string& path) {
    if (!std::filesystem::exists(path)) {
        return;
    }
    std::ifstream file(path);
    if (!file) return;
    nlohmann::json j;
    file >> j;
    std::vector<Order> orders;
    for (const auto& item : j["orders"]) {
        orders.push_back(orderFromJson(item));
    }
    int nextId = j.value("nextOrderId", 1);
    model.restore(std::move(orders), nextId);
}

void OrderRepository::save(const OrderModel& model, const std::string& path) {
    std::filesystem::path p(path);
    if (p.has_parent_path()) {
        std::filesystem::create_directories(p.parent_path());
    }
    nlohmann::json j;
    j["nextOrderId"] = model.getNextId();
    j["orders"] = nlohmann::json::array();
    for (const auto& o : model.getAll()) {
        j["orders"].push_back(toJson(o));
    }
    std::ofstream file(path);
    if (!file) return;
    file << j.dump(2);
}
