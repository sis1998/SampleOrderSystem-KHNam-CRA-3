#include <gtest/gtest.h>
#include <sstream>
#include "../src/model/SampleModel.h"
#include "../src/model/OrderModel.h"
#include "../src/model/ProductionQueue.h"
#include "../src/controller/OrderController.h"

// 유효한 시료가 있을 때 placeOrder는 RESERVED 상태의 주문을 생성한다
TEST(OrderControllerTest, PlaceOrder_ValidSample_CreatesReservedOrder) {
    SampleModel sampleModel;
    sampleModel.add({"S-001", "Silicon", 0.5, 0.92, 100});

    OrderModel orderModel;
    ProductionQueue queue;
    OrderController controller(sampleModel, orderModel, queue);

    // 입력: sampleId, 고객명, 수량
    std::istringstream in("S-001\n홍길동\n10\n");
    std::ostringstream out;
    controller.placeOrder(in, out);

    auto reserved = orderModel.getByStatus(OrderStatus::RESERVED);
    ASSERT_EQ(reserved.size(), 1u);
    EXPECT_EQ(reserved[0].sampleId, "S-001");
    EXPECT_EQ(reserved[0].customerName, "홍길동");
    EXPECT_EQ(reserved[0].quantity, 10);
}

// 재고가 충분할 때 승인하면 CONFIRMED 상태로 전환된다
TEST(OrderControllerTest, Approve_StockSufficient_TransitionsToConfirmed) {
    SampleModel sampleModel;
    sampleModel.add({"S-001", "Silicon", 0.5, 0.92, 100});

    OrderModel orderModel;
    orderModel.create("S-001", "홍길동", 10);

    ProductionQueue queue;
    OrderController controller(sampleModel, orderModel, queue);

    // 입력: 번호 선택 "1", 승인 "승인"
    std::istringstream in("1\n승인\n");
    std::ostringstream out;
    controller.approveReject(in, out);

    auto confirmed = orderModel.getByStatus(OrderStatus::CONFIRMED);
    ASSERT_EQ(confirmed.size(), 1u);
    EXPECT_EQ(confirmed[0].sampleId, "S-001");
}

// 재고가 부족할 때 승인하면 PRODUCING 상태로 전환된다
TEST(OrderControllerTest, Approve_StockInsufficient_TransitionsToProducing) {
    SampleModel sampleModel;
    sampleModel.add({"S-001", "Silicon", 0.5, 0.92, 10});

    OrderModel orderModel;
    orderModel.create("S-001", "홍길동", 50);

    ProductionQueue queue;
    OrderController controller(sampleModel, orderModel, queue);

    // 입력: 번호 선택 "1", 승인 "승인"
    std::istringstream in("1\n승인\n");
    std::ostringstream out;
    controller.approveReject(in, out);

    auto producing = orderModel.getByStatus(OrderStatus::PRODUCING);
    ASSERT_EQ(producing.size(), 1u);
    EXPECT_EQ(producing[0].sampleId, "S-001");
}

// 거절하면 REJECTED 상태로 전환된다
TEST(OrderControllerTest, Reject_TransitionsToRejected) {
    SampleModel sampleModel;
    sampleModel.add({"S-001", "Silicon", 0.5, 0.92, 100});

    OrderModel orderModel;
    orderModel.create("S-001", "홍길동", 10);

    ProductionQueue queue;
    OrderController controller(sampleModel, orderModel, queue);

    // 입력: 번호 선택 "1", 거절 "거절"
    std::istringstream in("1\n거절\n");
    std::ostringstream out;
    controller.approveReject(in, out);

    auto rejected = orderModel.getByStatus(OrderStatus::REJECTED);
    ASSERT_EQ(rejected.size(), 1u);
}

// CONFIRMED 주문을 출고 처리하면 RELEASE 상태로 전환된다
TEST(OrderControllerTest, Release_ConfirmedOrder_TransitionsToRelease) {
    SampleModel sampleModel;
    sampleModel.add({"S-001", "Silicon", 0.5, 0.92, 100});

    OrderModel orderModel;
    std::string orderId = orderModel.create("S-001", "홍길동", 10);
    orderModel.transitionTo(orderId, OrderStatus::CONFIRMED);

    ProductionQueue queue;
    OrderController controller(sampleModel, orderModel, queue);

    // 입력: 번호 선택 "1"
    std::istringstream in("1\n");
    std::ostringstream out;
    controller.release(in, out);

    auto released = orderModel.getByStatus(OrderStatus::RELEASE);
    ASSERT_EQ(released.size(), 1u);
}
