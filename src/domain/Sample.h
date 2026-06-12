#pragma once

#include <string>

namespace domain {

struct Sample {
    std::string sampleId;          // user-assigned, e.g. "S-001"
    std::string name;
    double      avgProductionTime; // minutes per each
    double      yield;             // 0 < yield <= 1
    int         stock;
};

} // namespace domain
