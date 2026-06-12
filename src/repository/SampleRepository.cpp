#include "SampleRepository.h"
#include "../infra/JsonSerializer.h"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>

void SampleRepository::load(SampleModel& model, const std::string& path) {
    if (!std::filesystem::exists(path)) {
        return;
    }
    std::ifstream file(path);
    if (!file) return;
    nlohmann::json j;
    file >> j;
    for (const auto& item : j["samples"]) {
        model.add(sampleFromJson(item));
    }
}

void SampleRepository::save(const SampleModel& model, const std::string& path) {
    std::filesystem::path p(path);
    if (p.has_parent_path()) {
        std::filesystem::create_directories(p.parent_path());
    }
    nlohmann::json j;
    j["samples"] = nlohmann::json::array();
    for (const auto& s : model.getAll()) {
        j["samples"].push_back(toJson(s));
    }
    std::ofstream file(path);
    if (!file) return;
    file << j.dump(2);
}
