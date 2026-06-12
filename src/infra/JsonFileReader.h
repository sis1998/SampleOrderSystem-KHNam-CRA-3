#pragma once
#include <optional>
#include <string>

namespace oss::infra {

class JsonFileReader {
public:
    std::optional<std::string> read(const std::string& path) const;
};

} // namespace oss::infra
