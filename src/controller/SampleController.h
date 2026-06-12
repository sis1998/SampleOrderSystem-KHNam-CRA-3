#pragma once
#include <istream>
#include <ostream>
#include "../model/SampleModel.h"
#include "../view/SampleView.h"

class SampleController {
public:
    explicit SampleController(SampleModel& model);
    void run(std::istream& in, std::ostream& out);
private:
    SampleModel& model_;
    SampleView view_;
};
