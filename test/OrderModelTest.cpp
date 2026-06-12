#include <gtest/gtest.h>
#include "../src/model/OrderModel.h"

TEST(OrderModelTest, Create_FirstOrder_ReturnsORD1) {
    OrderModel model;
    std::string id = model.create("S001", "Alice", 5);
    EXPECT_EQ(id, "ORD-1");
}

TEST(OrderModelTest, Create_SecondOrder_ReturnsORD2) {
    OrderModel model;
    model.create("S001", "Alice", 5);
    std::string id = model.create("S002", "Bob", 3);
    EXPECT_EQ(id, "ORD-2");
}

TEST(OrderModelTest, GetByStatus_ReturnsOnlyMatchingStatus) {
    OrderModel model;
    model.create("S001", "Alice", 5);
    std::string id2 = model.create("S002", "Bob", 3);
    model.transitionTo(id2, OrderStatus::CONFIRMED);

    auto reserved = model.getByStatus(OrderStatus::RESERVED);
    ASSERT_EQ(reserved.size(), 1u);
    EXPECT_EQ(reserved[0].orderId, "ORD-1");

    auto confirmed = model.getByStatus(OrderStatus::CONFIRMED);
    ASSERT_EQ(confirmed.size(), 1u);
    EXPECT_EQ(confirmed[0].orderId, "ORD-2");
}

TEST(OrderModelTest, TransitionTo_ValidTransitions) {
    // RESERVED -> CONFIRMED
    {
        OrderModel model;
        std::string id = model.create("S001", "Alice", 1);
        EXPECT_TRUE(model.transitionTo(id, OrderStatus::CONFIRMED));
        EXPECT_EQ(model.findById(id)->status, OrderStatus::CONFIRMED);
    }
    // RESERVED -> PRODUCING
    {
        OrderModel model;
        std::string id = model.create("S001", "Alice", 1);
        EXPECT_TRUE(model.transitionTo(id, OrderStatus::PRODUCING));
        EXPECT_EQ(model.findById(id)->status, OrderStatus::PRODUCING);
    }
    // RESERVED -> REJECTED
    {
        OrderModel model;
        std::string id = model.create("S001", "Alice", 1);
        EXPECT_TRUE(model.transitionTo(id, OrderStatus::REJECTED));
        EXPECT_EQ(model.findById(id)->status, OrderStatus::REJECTED);
    }
    // PRODUCING -> CONFIRMED
    {
        OrderModel model;
        std::string id = model.create("S001", "Alice", 1);
        model.transitionTo(id, OrderStatus::PRODUCING);
        EXPECT_TRUE(model.transitionTo(id, OrderStatus::CONFIRMED));
        EXPECT_EQ(model.findById(id)->status, OrderStatus::CONFIRMED);
    }
    // CONFIRMED -> RELEASE
    {
        OrderModel model;
        std::string id = model.create("S001", "Alice", 1);
        model.transitionTo(id, OrderStatus::CONFIRMED);
        EXPECT_TRUE(model.transitionTo(id, OrderStatus::RELEASE));
        EXPECT_EQ(model.findById(id)->status, OrderStatus::RELEASE);
    }
}

TEST(OrderModelTest, TransitionTo_InvalidTransition_ReturnsFalse) {
    OrderModel model;
    std::string id = model.create("S001", "Alice", 1);
    model.transitionTo(id, OrderStatus::CONFIRMED);
    // CONFIRMED -> RESERVED is not allowed
    EXPECT_FALSE(model.transitionTo(id, OrderStatus::RESERVED));
    EXPECT_EQ(model.findById(id)->status, OrderStatus::CONFIRMED);
}

TEST(OrderModelTest, TerminalStates_CannotTransition) {
    // REJECTED is terminal
    {
        OrderModel model;
        std::string id = model.create("S001", "Alice", 1);
        model.transitionTo(id, OrderStatus::REJECTED);
        EXPECT_FALSE(model.transitionTo(id, OrderStatus::RESERVED));
        EXPECT_FALSE(model.transitionTo(id, OrderStatus::CONFIRMED));
        EXPECT_FALSE(model.transitionTo(id, OrderStatus::PRODUCING));
        EXPECT_FALSE(model.transitionTo(id, OrderStatus::RELEASE));
    }
    // RELEASE is terminal
    {
        OrderModel model;
        std::string id = model.create("S001", "Alice", 1);
        model.transitionTo(id, OrderStatus::CONFIRMED);
        model.transitionTo(id, OrderStatus::RELEASE);
        EXPECT_FALSE(model.transitionTo(id, OrderStatus::RESERVED));
        EXPECT_FALSE(model.transitionTo(id, OrderStatus::CONFIRMED));
        EXPECT_FALSE(model.transitionTo(id, OrderStatus::PRODUCING));
        EXPECT_FALSE(model.transitionTo(id, OrderStatus::REJECTED));
    }
}
