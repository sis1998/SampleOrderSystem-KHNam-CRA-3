#include "MonitorController.h"
#include <map>
#include <string>
#include "../domain/Order.h"

MonitorController::MonitorController(SampleModel& sampleModel, OrderModel& orderModel)
    : sampleModel_(sampleModel), orderModel_(orderModel) {}

void MonitorController::run(std::istream& in, std::ostream& out) {
    auto reserved  = orderModel_.getByStatus(OrderStatus::RESERVED);
    auto producing = orderModel_.getByStatus(OrderStatus::PRODUCING);
    auto confirmed = orderModel_.getByStatus(OrderStatus::CONFIRMED);
    auto released  = orderModel_.getByStatus(OrderStatus::RELEASE);

    std::map<OrderStatus, int> counts;
    counts[OrderStatus::RESERVED]  = static_cast<int>(reserved.size());
    counts[OrderStatus::PRODUCING] = static_cast<int>(producing.size());
    counts[OrderStatus::CONFIRMED] = static_cast<int>(confirmed.size());
    counts[OrderStatus::RELEASE]   = static_cast<int>(released.size());

    std::vector<Order> allActiveOrders;
    allActiveOrders.insert(allActiveOrders.end(), reserved.begin(),  reserved.end());
    allActiveOrders.insert(allActiveOrders.end(), producing.begin(), producing.end());
    allActiveOrders.insert(allActiveOrders.end(), confirmed.begin(), confirmed.end());
    allActiveOrders.insert(allActiveOrders.end(), released.begin(),  released.end());

    view_.renderOrderCounts(out, counts);
    view_.renderStockStatus(out, sampleModel_.getAll(), allActiveOrders);

    out << "계속하려면 Enter를 누르세요...";
    std::string dummy;
    std::getline(in, dummy);
}
