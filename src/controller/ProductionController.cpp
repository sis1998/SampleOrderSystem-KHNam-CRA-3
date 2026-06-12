#include "ProductionController.h"
#include <string>

ProductionController::ProductionController(ProductionQueue& queue)
    : queue_(queue) {}

void ProductionController::run(std::istream& in, std::ostream& out) {
    view_.renderQueue(out, queue_.getCurrent(), queue_.getWaiting());

    out << "계속하려면 Enter를 누르세요...";
    std::string dummy;
    std::getline(in, dummy);
}
