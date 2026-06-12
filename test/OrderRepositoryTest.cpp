#include <gtest/gtest.h>
#include "../src/repository/OrderRepository.h"
#include "../src/model/OrderModel.h"
#include <cstdio>

TEST(OrderRepositoryTest, SaveAndLoad_Roundtrip) {
    OrderModel m;
    m.create("S-001", "홍길동", 30);
    m.create("S-002", "김철수", 50);
    m.transitionTo("ORD-1", OrderStatus::CONFIRMED);

    OrderRepository repo;
    repo.save(m, "test_orders_tmp.json");

    OrderModel m2;
    repo.load(m2, "test_orders_tmp.json");

    ASSERT_EQ(m2.getByStatus(OrderStatus::CONFIRMED).size(), 1u);
    ASSERT_EQ(m2.getByStatus(OrderStatus::RESERVED).size(), 1u);
    EXPECT_EQ(m2.findById("ORD-1")->quantity, 30);
    EXPECT_EQ(m2.findById("ORD-2")->customerName, "김철수");

    // nextId 복원 확인: 다음 주문은 ORD-3
    std::string newId = m2.create("S-001", "박지수", 10);
    EXPECT_EQ(newId, "ORD-3");

    std::remove("test_orders_tmp.json");
}

TEST(OrderRepositoryTest, Load_FileNotExists_NoOp) {
    OrderModel m;
    OrderRepository repo;
    repo.load(m, "nonexistent_orders.json");
    EXPECT_TRUE(m.getByStatus(OrderStatus::RESERVED).empty());
}
