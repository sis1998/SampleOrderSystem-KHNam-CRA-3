#pragma once
#include <ostream>

class MainView {
public:
    void renderHeader(std::ostream& os, int sampleCount, int totalStock, int orderCount, int queueSize);
    void renderMenu(std::ostream& os);
};
