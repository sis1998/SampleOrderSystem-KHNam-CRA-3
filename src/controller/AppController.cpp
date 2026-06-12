#include "AppController.h"
#include <iostream>
#include <string>
#include <numeric>

AppController::AppController()
    : sampleCtrl_(sampleModel_)
    , orderCtrl_(sampleModel_, orderModel_, productionQueue_)
    , monitorCtrl_(sampleModel_, orderModel_)
    , productionCtrl_(productionQueue_)
{
    // 시드 데이터
    Sample s1;
    s1.sampleId = "S-001";
    s1.name = "실리콘 웨이퍼-8인치";
    s1.avgProductionTime = 0.5;
    s1.yield = 0.92;
    s1.stock = 480;
    sampleModel_.add(s1);

    Sample s2;
    s2.sampleId = "S-002";
    s2.name = "GaAs 기판-4인치";
    s2.avgProductionTime = 1.2;
    s2.yield = 0.85;
    s2.stock = 120;
    sampleModel_.add(s2);

    Sample s3;
    s3.sampleId = "S-003";
    s3.name = "SiC 웨이퍼-6인치";
    s3.avgProductionTime = 2.0;
    s3.yield = 0.78;
    s3.stock = 60;
    sampleModel_.add(s3);
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

        int activeOrders = 0;
        for (const auto& o : orderModel_.getByStatus(OrderStatus::RESERVED)) {
            (void)o;
            ++activeOrders;
        }
        for (const auto& o : orderModel_.getByStatus(OrderStatus::PRODUCING)) {
            (void)o;
            ++activeOrders;
        }
        for (const auto& o : orderModel_.getByStatus(OrderStatus::CONFIRMED)) {
            (void)o;
            ++activeOrders;
        }

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
}
