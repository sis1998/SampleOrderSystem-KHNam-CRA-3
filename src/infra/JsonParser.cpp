#include "JsonParser.h"
#include <algorithm>
#include <cctype>

namespace oss::infra {

namespace {

bool isBlank(const std::string& str) {
    return std::all_of(str.cbegin(), str.cend(), [](unsigned char c) {
        return std::isspace(c);
    });
}

} // namespace

std::optional<nlohmann::json> JsonParser::parse(const std::string& jsonString) const {
    if (jsonString.empty() || isBlank(jsonString)) {
        return std::nullopt;
    }
    try {
        return nlohmann::json::parse(jsonString);
    } catch (...) {
        return std::nullopt;
    }
}

} // namespace oss::infra
