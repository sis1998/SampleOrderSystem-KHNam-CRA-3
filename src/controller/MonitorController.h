#pragma once
#include <istream>
#include <ostream>
#include "../model/SampleModel.h"
#include "../model/OrderModel.h"
#include "../view/MonitorView.h"

class MonitorController {
public:
    MonitorController(SampleModel& sampleModel, OrderModel& orderModel);
    void run(std::istream& in, std::ostream& out);
private:
    SampleModel& sampleModel_;
    OrderModel& orderModel_;
    MonitorView view_;
};
