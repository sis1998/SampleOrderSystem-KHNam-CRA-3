#pragma once
#include <ostream>
#include <string_view>
#include <vector>
#include "../domain/Sample.h"

class SampleView {
public:
    void renderList(std::ostream& os, const std::vector<Sample>& samples);
    void renderMessage(std::ostream& os, std::string_view message);
};
