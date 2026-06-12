#include <gtest/gtest.h>
#include <sstream>
#include <thread>
#include <chrono>
#include "../src/model/SampleModel.h"
#include "../src/model/OrderModel.h"
#include "../src/model/ProductionQueue.h"
#include "../src/controller/OrderController.h"

TEST(OrderControllerTest, PlaceOrder_ValidSample_CreatesReservedOrder) {
    SampleModel sm; sm.add({"S-001", "Silicon", 0.5, 0.92, 100});
    OrderModel om; ProductionQueue q;
    OrderController ctrl(sm, om, q);
    std::istringstream in("S-001\n홍길동\n10\n");
    std::ostringstream out;
    ctrl.placeOrder(in, out);
    auto reserved = om.getByStatus(OrderStatus::RESERVED);
    ASSERT_EQ(reserved.size(), 1u);
    EXPECT_EQ(reserved[0].quantity, 10);
}

TEST(OrderControllerTest, Approve_StockSufficient_TransitionsToConfirmed_DeductsStock) {
    SampleModel sm; sm.add({"S-001", "Silicon", 0.5, 0.92, 100});
    OrderModel om; om.create("S-001", "홍길동", 30);
    ProductionQueue q;
    OrderController ctrl(sm, om, q);
    std::istringstream in("1\n승인\n");
    std::ostringstream out;
    ctrl.approveReject(in, out);
    ASSERT_EQ(om.getByStatus(OrderStatus::CONFIRMED).size(), 1u);
    // 재고 즉시 차감 (100 - 30 = 70)
    EXPECT_EQ(sm.findById("S-001")->stock, 70);
}

TEST(OrderControllerTest, Approve_StockInsufficient_TransitionsToProducing_NoDeduction) {
    SampleModel sm; sm.add({"S-001", "Silicon", 0.5, 0.92, 10});
    OrderModel om; om.create("S-001", "홍길동", 50);
    ProductionQueue q;
    OrderController ctrl(sm, om, q);
    std::istringstream in("1\n승인\n");
    std::ostringstream out;
    ctrl.approveReject(in, out);
    ASSERT_EQ(om.getByStatus(OrderStatus::PRODUCING).size(), 1u);
    // 재고 차감 없음
    EXPECT_EQ(sm.findById("S-001")->stock, 10);
}

TEST(OrderControllerTest, Reject_TransitionsToRejected) {
    SampleModel sm; sm.add({"S-001", "Silicon", 0.5, 0.92, 100});
    OrderModel om; om.create("S-001", "홍길동", 10);
    ProductionQueue q;
    OrderController ctrl(sm, om, q);
    std::istringstream in("1\n거절\n");
    std::ostringstream out;
    ctrl.approveReject(in, out);
    ASSERT_EQ(om.getByStatus(OrderStatus::REJECTED).size(), 1u);
}

// CONFIRMED (producing path): release 시 재고 차감
TEST(OrderControllerTest, Release_ProducingPath_DeductsStockAtRelease) {
    SampleModel sm; sm.add({"S-001", "Silicon", 0.5, 0.92, 60});
    OrderModel om;
    std::string id = om.create("S-001", "홍길동", 50);
    om.transitionTo(id, OrderStatus::CONFIRMED); // stockPreDeducted=false (producing path)
    ProductionQueue q;
    OrderController ctrl(sm, om, q);
    std::istringstream in("1\n");
    std::ostringstream out;
    ctrl.release(in, out);
    ASSERT_EQ(om.getByStatus(OrderStatus::RELEASE).size(), 1u);
    // 출고 시 차감 (60 - 50 = 10)
    EXPECT_EQ(sm.findById("S-001")->stock, 10);
}

// 경계값: stock == qty → CONFIRMED, 재고 정확히 0
TEST(OrderControllerTest, Approve_StockExactlyEqualsQty_TransitionsToConfirmed) {
    SampleModel sm; sm.add({"S-001", "Silicon", 0.5, 0.92, 30});
    OrderModel om; om.create("S-001", "홍길동", 30);
    ProductionQueue q;
    OrderController ctrl(sm, om, q);
    std::istringstream in("1\n승인\n");
    std::ostringstream out;
    ctrl.approveReject(in, out);
    ASSERT_EQ(om.getByStatus(OrderStatus::CONFIRMED).size(), 1u);
    EXPECT_EQ(sm.findById("S-001")->stock, 0);
}

// 생산 완료 후 잉여 재고: PRODUCING → tick → CONFIRMED → RELEASE
// stock=10, qty=12, shortfall=2, yield=0.9, avgTime=0.0001
// actualQty=ceil(2/0.81)=3, estimatedMinutes=0.0003min=0.018sec → 150ms sleep 충분
TEST(OrderControllerTest, ProducingPath_EndToEnd_StockCorrectAfterRelease) {
    SampleModel sm; sm.add({"S-001", "Silicon", 0.0001, 0.9, 10});
    OrderModel om;
    om.create("S-001", "홍길동", 12); // shortfall=2
    ProductionQueue q;
    OrderController ctrl(sm, om, q);

    // 승인 (재고 부족 → PRODUCING)
    { std::istringstream in("1\n승인\n"); std::ostringstream out; ctrl.approveReject(in, out); }
    ASSERT_EQ(om.getByStatus(OrderStatus::PRODUCING).size(), 1u);
    EXPECT_EQ(sm.findById("S-001")->stock, 10); // 차감 없음

    // 생산 완료 대기
    q.tick(sm, om); // start production
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    q.tick(sm, om); // complete: addStock(3), order→CONFIRMED
    ASSERT_EQ(om.getByStatus(OrderStatus::CONFIRMED).size(), 1u);
    EXPECT_EQ(sm.findById("S-001")->stock, 13); // 10 + 3 = 13

    // 출고 (재고 차감: qty=12, 잉여 1 보존)
    { std::istringstream in("1\n"); std::ostringstream out; ctrl.release(in, out); }
    ASSERT_EQ(om.getByStatus(OrderStatus::RELEASE).size(), 1u);
    EXPECT_EQ(sm.findById("S-001")->stock, 1); // 13 - 12 = 1 (잉여)
}

// CONFIRMED (sufficient path): release 시 추가 차감 없음
TEST(OrderControllerTest, Release_SufficientPath_NoDoubleDeduction) {
    SampleModel sm; sm.add({"S-001", "Silicon", 0.5, 0.92, 100});
    OrderModel om; om.create("S-001", "홍길동", 30);
    ProductionQueue q;
    OrderController ctrl(sm, om, q);
    // 승인 (재고 즉시 차감)
    { std::istringstream in("1\n승인\n"); std::ostringstream out; ctrl.approveReject(in, out); }
    EXPECT_EQ(sm.findById("S-001")->stock, 70);
    // 출고 (추가 차감 없음)
    { std::istringstream in("1\n"); std::ostringstream out; ctrl.release(in, out); }
    EXPECT_EQ(sm.findById("S-001")->stock, 70); // unchanged
}
