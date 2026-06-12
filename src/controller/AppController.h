#pragma once
#include "../model/SampleModel.h"
#include "../model/OrderModel.h"
#include "../model/ProductionQueue.h"
#include "../view/MainView.h"
#include "SampleController.h"
#include "OrderController.h"
#include "MonitorController.h"
#include "ProductionController.h"

class AppController {
public:
    AppController();
    void run();
private:
    SampleModel sampleModel_;
    OrderModel orderModel_;
    ProductionQueue productionQueue_;
    MainView mainView_;
    SampleController sampleCtrl_;
    OrderController orderCtrl_;
    MonitorController monitorCtrl_;
    ProductionController productionCtrl_;

    void dispatch(const std::string& input);
};
