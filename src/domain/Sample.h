#pragma once
#include <string>

struct Sample {
    std::string sampleId;
    std::string name;
    double avgProductionTime = 0.0;
    double yield = 1.0;
    int stock = 0;
};
