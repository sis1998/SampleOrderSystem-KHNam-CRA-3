#pragma once
#include <string>
#include "../model/ProductionQueue.h"

class ProductionQueueRepository {
public:
    void load(ProductionQueue& queue, const std::string& path);
    void save(const ProductionQueue& queue, const std::string& path);
};
