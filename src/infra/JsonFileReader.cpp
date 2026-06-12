#include "JsonFileReader.h"
#include <fstream>
#include <sstream>

namespace oss::infra {

std::optional<std::string> JsonFileReader::read(const std::string& path) const {
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            return std::nullopt;
        }
        std::ostringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    } catch (...) {
        return std::nullopt;
    }
}

} // namespace oss::infra
