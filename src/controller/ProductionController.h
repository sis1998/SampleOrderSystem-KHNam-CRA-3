#pragma once
#include <istream>
#include <ostream>
#include "../model/ProductionQueue.h"
#include "../view/ProductionView.h"

class ProductionController {
public:
    explicit ProductionController(ProductionQueue& queue);
    void run(std::istream& in, std::ostream& out);
private:
    ProductionQueue& queue_;
    ProductionView view_;
};
