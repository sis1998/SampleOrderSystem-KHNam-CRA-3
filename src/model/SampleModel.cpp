#include "SampleModel.h"
#include <algorithm>
#include <cctype>

bool SampleModel::add(const Sample& s) {
    for (const auto& item : items_) {
        if (item.sampleId == s.sampleId)
            return false;
    }
    items_.push_back(s);
    return true;
}

std::optional<Sample> SampleModel::findById(const std::string& id) const {
    for (const auto& item : items_) {
        if (item.sampleId == id)
            return item;
    }
    return std::nullopt;
}

std::vector<Sample> SampleModel::findByName(const std::string& query) const {
    auto toLower = [](const std::string& s) {
        std::string result = s;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return result;
    };

    std::string lowerQuery = toLower(query);
    std::vector<Sample> results;
    for (const auto& item : items_) {
        if (toLower(item.name).find(lowerQuery) != std::string::npos)
            results.push_back(item);
    }
    return results;
}

std::vector<Sample> SampleModel::getAll() const {
    return items_;
}

bool SampleModel::deductStock(const std::string& id, int qty) {
    for (auto& item : items_) {
        if (item.sampleId == id) {
            if (item.stock < qty)
                return false;
            item.stock -= qty;
            return true;
        }
    }
    return false;
}

void SampleModel::addStock(const std::string& id, int qty) {
    for (auto& item : items_) {
        if (item.sampleId == id) {
            item.stock += qty;
            return;
        }
    }
}
