#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "../src/model/ProductionQueue.h"
#include "../src/model/SampleModel.h"
#include "../src/model/OrderModel.h"
#include "../src/domain/Sample.h"

TEST(ProductionQueueTest, Enqueue_ComputesActualQtyWithSafetyMargin) {
    // actualQty = ceil(50 / (0.9 * 0.9)) = ceil(61.728...) = 62
    ProductionQueue q;
    q.enqueue("ORD-1", "S-001", 50, 1.0, 0.9);

    auto waiting = q.getWaiting();
    ASSERT_EQ(waiting.size(), 1u);
    EXPECT_EQ(waiting[0].actualQty, 62);
}

TEST(ProductionQueueTest, Enqueue_MultipleOrders_PreservesFIFO) {
    ProductionQueue q;
    q.enqueue("ORD-1", "S-001", 10, 1.0, 0.9);
    q.enqueue("ORD-2", "S-002", 10, 1.0, 0.9);

    auto waiting = q.getWaiting();
    ASSERT_EQ(waiting.size(), 2u);
    EXPECT_EQ(waiting[0].orderId, "ORD-1");
}

TEST(ProductionQueueTest, Tick_WhenWaiting_StartsNextItem) {
    ProductionQueue q;
    q.enqueue("ORD-1", "S-001", 10, 1.0, 0.9);

    SampleModel sm;
    OrderModel om;
    q.tick(sm, om);

    EXPECT_TRUE(q.getCurrent().has_value());
    EXPECT_TRUE(q.getWaiting().empty());
}

TEST(ProductionQueueTest, Tick_WhenElapsed_CompletesAndUpdatesModelState) {
    // actualQty = ceil(1 / (0.9 * 0.9)) = ceil(1.234...) = 2
    // estimatedMinutes = 0.0001 * 2 = 0.0002 min = ~0.012 seconds
    // sleep 150ms -> well past completion
    ProductionQueue q;
    q.enqueue("ORD-1", "S-001", 1, 0.0001, 0.9);

    SampleModel sm;
    Sample s;
    s.sampleId = "S-001";
    s.name = "TestSample";
    s.stock = 0;
    sm.add(s);

    OrderModel om;
    std::string orderId = om.create("S-001", "TestCustomer", 1);

    // First tick: start the item
    q.tick(sm, om);
    ASSERT_TRUE(q.getCurrent().has_value());

    // Wait past estimatedMinutes
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    // Second tick: complete and update models
    bool completed = q.tick(sm, om);
    EXPECT_TRUE(completed);
    EXPECT_FALSE(q.getCurrent().has_value());

    auto updatedOrder = om.findById(orderId);
    ASSERT_TRUE(updatedOrder.has_value());
    EXPECT_EQ(updatedOrder->status, OrderStatus::CONFIRMED);

    auto updatedSample = sm.findById("S-001");
    ASSERT_TRUE(updatedSample.has_value());
    EXPECT_EQ(updatedSample->stock, 2);
}
