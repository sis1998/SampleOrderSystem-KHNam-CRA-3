#include <gtest/gtest.h>
#include "../src/model/SampleModel.h"

TEST(SampleModelTest, Add_StoresAndRetrievesById) {
    SampleModel model;
    Sample s;
    s.sampleId = "S001";
    s.name = "Widget";
    s.stock = 10;

    EXPECT_TRUE(model.add(s));
    auto result = model.findById("S001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->sampleId, "S001");
    EXPECT_EQ(result->name, "Widget");
    EXPECT_EQ(result->stock, 10);
}

TEST(SampleModelTest, Add_DuplicateId_ReturnsFalse) {
    SampleModel model;
    Sample s;
    s.sampleId = "S001";
    s.name = "Widget";

    EXPECT_TRUE(model.add(s));
    EXPECT_FALSE(model.add(s));
}

TEST(SampleModelTest, FindByName_ReturnsSubstringMatch) {
    SampleModel model;
    Sample s1;
    s1.sampleId = "S001";
    s1.name = "Widget Alpha";

    Sample s2;
    s2.sampleId = "S002";
    s2.name = "Gadget Beta";

    Sample s3;
    s3.sampleId = "S003";
    s3.name = "widget gamma";

    model.add(s1);
    model.add(s2);
    model.add(s3);

    auto results = model.findByName("widget");
    ASSERT_EQ(results.size(), 2u);
}

TEST(SampleModelTest, DeductStock_ReducesStock) {
    SampleModel model;
    Sample s;
    s.sampleId = "S001";
    s.name = "Widget";
    s.stock = 10;
    model.add(s);

    EXPECT_TRUE(model.deductStock("S001", 3));
    auto result = model.findById("S001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->stock, 7);
}

TEST(SampleModelTest, DeductStock_Insufficient_ReturnsFalse) {
    SampleModel model;
    Sample s;
    s.sampleId = "S001";
    s.name = "Widget";
    s.stock = 2;
    model.add(s);

    EXPECT_FALSE(model.deductStock("S001", 5));
    auto result = model.findById("S001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->stock, 2);
}

TEST(SampleModelTest, AddStock_IncreasesStock) {
    SampleModel model;
    Sample s;
    s.sampleId = "S001";
    s.name = "Widget";
    s.stock = 5;
    model.add(s);

    model.addStock("S001", 10);
    auto result = model.findById("S001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->stock, 15);
}
