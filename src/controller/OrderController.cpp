#include "OrderController.h"

OrderController::OrderController(SampleModel& sm, OrderModel& om, ProductionQueue& q)
    : sampleModel_(sm), orderModel_(om), queue_(q) {}

void OrderController::placeOrder(std::istream& in, std::ostream& out) {
    std::string sampleId, customerName, qtyStr;

    out << "시료 ID: ";
    std::getline(in, sampleId);
    out << "고객명: ";
    std::getline(in, customerName);
    out << "수량: ";
    std::getline(in, qtyStr);

    auto sample = sampleModel_.findById(sampleId);
    if (!sample) {
        view_.renderMessage(out, "시료를 찾을 수 없습니다.");
        return;
    }

    int quantity = std::stoi(qtyStr);
    std::string orderId = orderModel_.create(sampleId, customerName, quantity);
    view_.renderMessage(out, "주문 접수 완료. 주문 ID: " + orderId);
}

void OrderController::approveReject(std::istream& in, std::ostream& out) {
    auto reserved = orderModel_.getByStatus(OrderStatus::RESERVED);
    if (reserved.empty()) {
        view_.renderMessage(out, "승인 대기 주문이 없습니다.");
        return;
    }

    view_.renderReservedList(out, reserved);
    out << "번호 선택: ";
    std::string choiceStr;
    std::getline(in, choiceStr);
    int choice = std::stoi(choiceStr);

    if (choice < 1 || choice > static_cast<int>(reserved.size())) {
        view_.renderMessage(out, "잘못된 선택입니다.");
        return;
    }

    const Order& order = reserved[choice - 1];
    out << "승인/거절: ";
    std::string decision;
    std::getline(in, decision);

    if (decision == "승인") {
        auto sample = sampleModel_.findById(order.sampleId);
        int currentStock = sample ? sample->stock : 0;

        if (currentStock >= order.quantity) {
            sampleModel_.deductStock(order.sampleId, order.quantity);
            orderModel_.markStockDeducted(order.orderId);
            orderModel_.transitionTo(order.orderId, OrderStatus::CONFIRMED);
            view_.renderMessage(out, "승인 완료 (출고대기).");
        } else {
            int shortfall = order.quantity - currentStock;
            double avgTime = sample ? sample->avgProductionTime : 0.0;
            double yield   = sample ? sample->yield : 1.0;
            orderModel_.transitionTo(order.orderId, OrderStatus::PRODUCING);
            queue_.enqueue(order.orderId, order.sampleId, shortfall, avgTime, yield);
            view_.renderMessage(out, "재고 부족 — 생산 대기열 등록 (생산중).");
        }
    } else if (decision == "거절") {
        orderModel_.transitionTo(order.orderId, OrderStatus::REJECTED);
        view_.renderMessage(out, "주문 거절.");
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
    out << "번호 선택: ";
    std::string choiceStr;
    std::getline(in, choiceStr);
    int choice = std::stoi(choiceStr);

    if (choice < 1 || choice > static_cast<int>(confirmed.size())) {
        view_.renderMessage(out, "잘못된 선택입니다.");
        return;
    }

    const Order& order = confirmed[choice - 1];
    if (!order.stockPreDeducted) {
        sampleModel_.deductStock(order.sampleId, order.quantity);
    }
    orderModel_.transitionTo(order.orderId, OrderStatus::RELEASE);
    view_.renderMessage(out, "출고 완료.");
}
