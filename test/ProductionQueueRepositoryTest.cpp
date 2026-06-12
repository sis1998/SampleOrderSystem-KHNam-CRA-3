#include <gtest/gtest.h>
#include "../src/repository/ProductionQueueRepository.h"
#include "../src/model/ProductionQueue.h"
#include "../src/model/SampleModel.h"
#include "../src/model/OrderModel.h"
#include <cstdio>

TEST(ProductionQueueRepositoryTest, SaveAndLoad_WaitingItem) {
    SampleModel sm; sm.add({"S-001", "Silicon", 0.0001, 0.9, 10});
    OrderModel om; om.create("S-001", "홍길동", 20);
    ProductionQueue q;
    q.enqueue("ORD-1", "S-001", 10, 0.0001, 0.9);

    ProductionQueueRepository repo;
    repo.save(q, "test_queue_tmp.json");

    ProductionQueue q2;
    repo.load(q2, "test_queue_tmp.json");
    EXPECT_EQ(q2.getWaiting().size(), 1u);
    EXPECT_EQ(q2.getWaiting()[0].orderId, "ORD-1");
    EXPECT_EQ(q2.totalQueueSize(), 1);

    std::remove("test_queue_tmp.json");
}

TEST(ProductionQueueRepositoryTest, Load_FileNotExists_NoOp) {
    ProductionQueue q;
    ProductionQueueRepository repo;
    repo.load(q, "nonexistent_queue.json");
    EXPECT_EQ(q.totalQueueSize(), 0);
}
