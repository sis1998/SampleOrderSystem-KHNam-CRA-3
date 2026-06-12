#include <gtest/gtest.h>
#include <sstream>
#include "../src/model/SampleModel.h"
#include "../src/controller/SampleController.h"
#include "../src/view/SampleView.h"

TEST(SampleControllerTest, Input0_ExitsImmediately) {
    SampleModel model;
    SampleController controller(model);

    std::istringstream in("0\n");
    std::ostringstream out;
    controller.run(in, out);

    EXPECT_TRUE(model.getAll().empty());
}

TEST(SampleControllerTest, Input1_AddsValidSample) {
    SampleModel model;
    SampleController controller(model);

    // 1=시료 등록, then fields: sampleId, name, avgProductionTime, yield, stock, then 0=뒤로
    std::istringstream in("1\nS-001\nSilicon\n0.5\n0.92\n100\n0\n");
    std::ostringstream out;
    controller.run(in, out);

    auto all = model.getAll();
    ASSERT_EQ(all.size(), 1u);
    EXPECT_EQ(all[0].sampleId, "S-001");
    EXPECT_EQ(all[0].name, "Silicon");
}

TEST(SampleControllerTest, Input2_ListsAllSamples) {
    SampleModel model;
    model.add({"S-001", "Silicon", 0.5, 0.92, 100});
    model.add({"S-002", "Germanium", 1.0, 0.85, 50});

    SampleController controller(model);

    std::istringstream in("2\n0\n");
    std::ostringstream out;
    controller.run(in, out);

    std::string output = out.str();
    EXPECT_NE(output.find("Silicon"), std::string::npos);
    EXPECT_NE(output.find("Germanium"), std::string::npos);
}

TEST(SampleViewTest, RenderIndexedList_ShowsNumberedEntries) {
    SampleView view;
    std::vector<Sample> samples = {
        {"S-001", "Silicon",   0.5, 0.92, 100},
        {"S-002", "Germanium", 1.0, 0.85,  50},
    };
    std::ostringstream out;
    view.renderIndexedList(out, samples);
    std::string s = out.str();
    EXPECT_NE(s.find(" 1."), std::string::npos);
    EXPECT_NE(s.find(" 2."), std::string::npos);
    EXPECT_NE(s.find("S-001"), std::string::npos);
    EXPECT_NE(s.find("S-002"), std::string::npos);
}

TEST(SampleViewTest, RenderList_ShowsIdAndName) {
    SampleView view;
    std::vector<Sample> samples = {{"S-001", "Silicon", 0.5, 0.92, 100}};
    std::ostringstream out;
    view.renderList(out, samples);
    std::string s = out.str();
    EXPECT_NE(s.find("[S-001]"), std::string::npos);
    EXPECT_NE(s.find("Silicon"), std::string::npos);
    EXPECT_NE(s.find("100"), std::string::npos);
}
