#pragma once
#include <istream>
#include <ostream>
#include "../model/SampleModel.h"
#include "../model/OrderModel.h"
#include "../model/ProductionQueue.h"
#include "../view/OrderView.h"

class OrderController {
public:
    OrderController(SampleModel& sm, OrderModel& om, ProductionQueue& q);
    void placeOrder(std::istream& in, std::ostream& out);
    void approveReject(std::istream& in, std::ostream& out);
    void release(std::istream& in, std::ostream& out);
private:
    SampleModel& sampleModel_;
    OrderModel& orderModel_;
    ProductionQueue& queue_;
    OrderView view_;
};
