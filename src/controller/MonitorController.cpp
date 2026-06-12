#include "MonitorController.h"
#include <map>
#include <string>
#include "../domain/Order.h"

MonitorController::MonitorController(SampleModel& sampleModel, OrderModel& orderModel)
    : sampleModel_(sampleModel), orderModel_(orderModel) {}

void MonitorController::run(std::istream& in, std::ostream& out) {
    // 주문 상태별 개수 계산 (REJECTED 제외)
    std::map<OrderStatus, int> counts;
    counts[OrderStatus::RESERVED] = static_cast<int>(orderModel_.getByStatus(OrderStatus::RESERVED).size());
    counts[OrderStatus::PRODUCING] = static_cast<int>(orderModel_.getByStatus(OrderStatus::PRODUCING).size());
    counts[OrderStatus::CONFIRMED] = static_cast<int>(orderModel_.getByStatus(OrderStatus::CONFIRMED).size());
    counts[OrderStatus::RELEASE] = static_cast<int>(orderModel_.getByStatus(OrderStatus::RELEASE).size());

    // 모든 활성 주문 조회
    std::vector<Order> allActiveOrders;
    allActiveOrders.insert(allActiveOrders.end(),
        orderModel_.getByStatus(OrderStatus::RESERVED).begin(),
        orderModel_.getByStatus(OrderStatus::RESERVED).end());
    allActiveOrders.insert(allActiveOrders.end(),
        orderModel_.getByStatus(OrderStatus::PRODUCING).begin(),
        orderModel_.getByStatus(OrderStatus::PRODUCING).end());
    allActiveOrders.insert(allActiveOrders.end(),
        orderModel_.getByStatus(OrderStatus::CONFIRMED).begin(),
        orderModel_.getByStatus(OrderStatus::CONFIRMED).end());
    allActiveOrders.insert(allActiveOrders.end(),
        orderModel_.getByStatus(OrderStatus::RELEASE).begin(),
        orderModel_.getByStatus(OrderStatus::RELEASE).end());

    view_.renderOrderCounts(out, counts);
    view_.renderStockStatus(out, sampleModel_.getAll(), allActiveOrders);

    out << "계속하려면 Enter를 누르세요...";
    std::string dummy;
    std::getline(in, dummy);
}
