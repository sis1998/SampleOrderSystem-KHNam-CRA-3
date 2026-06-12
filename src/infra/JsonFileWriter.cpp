#include "JsonFileWriter.h"
#include <filesystem>
#include <fstream>

namespace oss::infra {

bool JsonFileWriter::write(const std::string& path, const std::string& content) const {
    try {
        const std::filesystem::path filePath(path);
        const auto parentDir = filePath.parent_path();
        if (!parentDir.empty()) {
            std::filesystem::create_directories(parentDir);
        }
        std::ofstream file(path, std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            return false;
        }
        file << content;
        file.flush();
        return file.good();
    } catch (...) {
        return false;
    }
}

} // namespace oss::infra
