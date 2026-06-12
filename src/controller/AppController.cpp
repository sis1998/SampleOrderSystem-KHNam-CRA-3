#include "AppController.h"
#include <iostream>
#include <string>

static const std::string SAMPLES_PATH = "data/samples.json";
static const std::string ORDERS_PATH  = "data/orders.json";
static const std::string QUEUE_PATH   = "data/production_queue.json";

AppController::AppController()
    : sampleCtrl_(sampleModel_)
    , orderCtrl_(sampleModel_, orderModel_, productionQueue_)
    , monitorCtrl_(sampleModel_, orderModel_)
    , productionCtrl_(productionQueue_)
{
    sampleRepo_.load(sampleModel_, SAMPLES_PATH);
    orderRepo_.load(orderModel_, ORDERS_PATH);
    queueRepo_.load(productionQueue_, QUEUE_PATH);
}

void AppController::run() {
    std::string input;
    while (true) {
        productionQueue_.tick(sampleModel_, orderModel_);

        auto allSamples = sampleModel_.getAll();
        int totalStock = 0;
        for (const auto& s : allSamples) {
            totalStock += s.stock;
        }

        int activeOrders =
            static_cast<int>(orderModel_.getByStatus(OrderStatus::RESERVED).size()) +
            static_cast<int>(orderModel_.getByStatus(OrderStatus::PRODUCING).size()) +
            static_cast<int>(orderModel_.getByStatus(OrderStatus::CONFIRMED).size());

        mainView_.renderHeader(std::cout,
                               static_cast<int>(allSamples.size()),
                               totalStock,
                               activeOrders,
                               productionQueue_.totalQueueSize());
        mainView_.renderMenu(std::cout);

        if (!std::getline(std::cin, input)) break;
        if (input == "0") break;
        dispatch(input);
    }
}

void AppController::dispatch(const std::string& input) {
    if (input == "1") {
        sampleCtrl_.run(std::cin, std::cout);
    } else if (input == "2") {
        orderCtrl_.placeOrder(std::cin, std::cout);
    } else if (input == "3") {
        orderCtrl_.approveReject(std::cin, std::cout);
    } else if (input == "4") {
        monitorCtrl_.run(std::cin, std::cout);
    } else if (input == "5") {
        productionCtrl_.run(std::cin, std::cout);
    } else if (input == "6") {
        orderCtrl_.release(std::cin, std::cout);
    }
    saveAll();
}

void AppController::saveAll() {
    sampleRepo_.save(sampleModel_, SAMPLES_PATH);
    orderRepo_.save(orderModel_, ORDERS_PATH);
    queueRepo_.save(productionQueue_, QUEUE_PATH);
}
