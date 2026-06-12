#pragma once
#include <optional>
#include <string>
#include <string_view>

inline std::optional<int> tryParseInt(std::string_view s) {
    try { return std::stoi(std::string(s)); } catch (...) { return std::nullopt; }
}

inline std::optional<double> tryParseDouble(std::string_view s) {
    try { return std::stod(std::string(s)); } catch (...) { return std::nullopt; }
}
