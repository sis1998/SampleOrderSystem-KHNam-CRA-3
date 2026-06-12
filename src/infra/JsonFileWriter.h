#pragma once
#include <string>

namespace oss::infra {

class JsonFileWriter {
public:
    bool write(const std::string& path, const std::string& content) const;
};

} // namespace oss::infra
