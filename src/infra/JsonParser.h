#pragma once
#include <optional>
#include <string>
#include "../../vendor/json.hpp"

namespace oss::infra {

class JsonParser {
public:
    std::optional<nlohmann::json> parse(const std::string& jsonString) const;
};

} // namespace oss::infra
