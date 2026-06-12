#include <gtest/gtest.h>
#include <sstream>
#include "../src/model/SampleModel.h"
#include "../src/model/OrderModel.h"
#include "../src/controller/MonitorController.h"

TEST(MonitorControllerTest, Run_ProducesOrderAndStockOutput) {
    SampleModel sm; sm.add({"S-001", "Silicon", 0.5, 0.92, 100});
    OrderModel om; om.create("S-001", "홍길동", 30);
    MonitorController ctrl(sm, om);
    std::istringstream in("\n");
    std::ostringstream out;
    ctrl.run(in, out);
    EXPECT_NE(out.str().find("주문 현황"), std::string::npos);
    EXPECT_NE(out.str().find("재고 현황"), std::string::npos);
    EXPECT_NE(out.str().find("접수"), std::string::npos);
}
