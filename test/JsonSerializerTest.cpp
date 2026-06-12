#include <gtest/gtest.h>
#include "../src/infra/JsonSerializer.h"

TEST(JsonSerializerTest, Sample_Roundtrip) {
    Sample s{"S-001","Silicon",0.5,0.92,100};
    EXPECT_EQ(sampleFromJson(toJson(s)).stock, 100);
    EXPECT_EQ(sampleFromJson(toJson(s)).sampleId, "S-001");
}

TEST(JsonSerializerTest, Order_Roundtrip_StatusConfirmed) {
    Order o; o.orderId="ORD-1"; o.status=OrderStatus::CONFIRMED; o.stockPreDeducted=true;
    auto o2 = orderFromJson(toJson(o));
    EXPECT_EQ(o2.status, OrderStatus::CONFIRMED);
    EXPECT_TRUE(o2.stockPreDeducted);
}

TEST(JsonSerializerTest, QueueItem_Roundtrip) {
    QueueItem q{"ORD-1","S-001",40,50,25.0};
    auto q2 = queueItemFromJson(toJson(q));
    EXPECT_EQ(q2.actualQty, 50);
    EXPECT_DOUBLE_EQ(q2.estimatedMinutes, 25.0);
}

TEST(JsonSerializerTest, ProductionItem_Roundtrip_ElapsedPreserved) {
    ProductionItem p;
    p.orderId = "ORD-1"; p.sampleId = "S-001";
    p.actualQty = 30; p.estimatedMinutes = 15.0;
    p.startedAt = std::chrono::steady_clock::now()
        - std::chrono::duration_cast<std::chrono::steady_clock::duration>(
            std::chrono::duration<double, std::ratio<60>>(5.0)); // 5분 전 시작
    auto p2 = productionItemFromJson(toJson(p));
    double elapsed = std::chrono::duration<double, std::ratio<60>>(
        std::chrono::steady_clock::now() - p2.startedAt).count();
    EXPECT_NEAR(elapsed, 5.0, 0.1); // 5분 ±0.1
}
