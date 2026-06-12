#include "ProductionQueueRepository.h"
#include "../infra/JsonSerializer.h"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>

void ProductionQueueRepository::load(ProductionQueue& queue, const std::string& path) {
    if (!std::filesystem::exists(path)) return;
    std::ifstream file(path);
    if (!file) return;

    nlohmann::json j;
    try {
        file >> j;
    } catch (const nlohmann::json::parse_error&) {
        return;
    }

    std::optional<ProductionItem> current;
    if (j.contains("current") && !j["current"].is_null()) {
        current = productionItemFromJson(j["current"]);
    }

    std::deque<QueueItem> waiting;
    if (j.contains("queue") && j["queue"].is_array()) {
        for (const auto& item : j["queue"]) {
            waiting.push_back(queueItemFromJson(item));
        }
    }

    queue.restore(std::move(current), std::move(waiting));
}

void ProductionQueueRepository::save(const ProductionQueue& queue, const std::string& path) {
    std::ofstream file(path);
    if (!file) return;

    nlohmann::json j;
    auto current = queue.getCurrent();
    if (current.has_value()) {
        j["current"] = toJson(*current);
    } else {
        j["current"] = nullptr;
    }

    j["queue"] = nlohmann::json::array();
    for (const auto& item : queue.getWaiting()) {
        j["queue"].push_back(toJson(item));
    }

    file << j.dump(2);
}
