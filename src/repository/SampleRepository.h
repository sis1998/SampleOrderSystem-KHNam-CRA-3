#pragma once
#include <string>
#include "../model/SampleModel.h"

class SampleRepository {
public:
    void load(SampleModel& model, const std::string& path);
    void save(const SampleModel& model, const std::string& path);
};
