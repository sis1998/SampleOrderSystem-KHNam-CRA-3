#include "OrderController.h"

OrderController::OrderController(SampleModel& sm, OrderModel& om, ProductionQueue& q)
    : sampleModel_(sm), orderModel_(om), queue_(q) {}

void OrderController::placeOrder(std::istream& in, std::ostream& out) {
    std::string sampleId, customerName;
    int quantity = 0;

    out << "시료 ID를 입력하세요: ";
    in >> sampleId;
    out << "고객명을 입력하세요: ";
    in >> customerName;
    out << "수량을 입력하세요: ";
    in >> quantity;

    auto sample = sampleModel_.findById(sampleId);
    if (!sample) {
        view_.renderMessage(out, "시료를 찾을 수 없습니다.");
        return;
    }

    std::string orderId = orderModel_.create(sampleId, customerName, quantity);
    view_.renderMessage(out, "주문이 접수되었습니다. 주문 ID: " + orderId);
}

void OrderController::approveReject(std::istream& in, std::ostream& out) {
    auto reserved = orderModel_.getByStatus(OrderStatus::RESERVED);
    if (reserved.empty()) {
        view_.renderMessage(out, "승인 대기 중인 주문이 없습니다.");
        return;
    }

    view_.renderReservedList(out, reserved);
    out << "번호를 선택하세요: ";

    int choice = 0;
    in >> choice;

    if (choice < 1 || choice > static_cast<int>(reserved.size())) {
        view_.renderMessage(out, "잘못된 선택입니다.");
        return;
    }

    const Order& order = reserved[choice - 1];
    out << "승인 또는 거절을 입력하세요 (승인/거절): ";
    std::string decision;
    in >> decision;

    if (decision == "승인") {
        auto sample = sampleModel_.findById(order.sampleId);
        int currentStock = sample ? sample->stock : 0;

        if (currentStock >= order.quantity) {
            sampleModel_.deductStock(order.sampleId, order.quantity);
            orderModel_.transitionTo(order.orderId, OrderStatus::CONFIRMED);
            view_.renderMessage(out, "주문이 승인되었습니다.");
        } else {
            int shortfall = order.quantity - currentStock;
            double avgTime = sample ? sample->avgProductionTime : 0.0;
            double yield = sample ? sample->yield : 1.0;
            orderModel_.transitionTo(order.orderId, OrderStatus::PRODUCING);
            queue_.enqueue(order.orderId, order.sampleId, shortfall, avgTime, yield);
            view_.renderMessage(out, "재고 부족으로 생산 대기열에 추가되었습니다.");
        }
    } else if (decision == "거절") {
        orderModel_.transitionTo(order.orderId, OrderStatus::REJECTED);
        view_.renderMessage(out, "주문이 거절되었습니다.");
    } else {
        view_.renderMessage(out, "잘못된 입력입니다.");
    }
}

void OrderController::release(std::istream& in, std::ostream& out) {
    auto confirmed = orderModel_.getByStatus(OrderStatus::CONFIRMED);
    if (confirmed.empty()) {
        view_.renderMessage(out, "출고 가능한 주문이 없습니다.");
        return;
    }

    view_.renderConfirmedList(out, confirmed);
    out << "번호를 선택하세요: ";

    int choice = 0;
    in >> choice;

    if (choice < 1 || choice > static_cast<int>(confirmed.size())) {
        view_.renderMessage(out, "잘못된 선택입니다.");
        return;
    }

    const Order& order = confirmed[choice - 1];
    orderModel_.transitionTo(order.orderId, OrderStatus::RELEASE);
    view_.renderMessage(out, "주문이 출고 처리되었습니다.");
}
