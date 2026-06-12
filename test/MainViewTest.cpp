#include <gtest/gtest.h>
#include <sstream>
#include "../src/view/MainView.h"

TEST(MainViewTest, RenderMenu_ContainsAllSixItems) {
    MainView view;
    std::ostringstream oss;
    view.renderMenu(oss);
    std::string output = oss.str();
    EXPECT_NE(output.find("1."), std::string::npos);
    EXPECT_NE(output.find("2."), std::string::npos);
    EXPECT_NE(output.find("3."), std::string::npos);
    EXPECT_NE(output.find("4."), std::string::npos);
    EXPECT_NE(output.find("5."), std::string::npos);
    EXPECT_NE(output.find("6."), std::string::npos);
    EXPECT_NE(output.find("0."), std::string::npos);
}

TEST(MainViewTest, RenderHeader_ContainsCounts) {
    MainView view;
    std::ostringstream oss;
    view.renderHeader(oss, 42, 100, 7, 3);
    std::string output = oss.str();
    EXPECT_NE(output.find("42"), std::string::npos);
    EXPECT_NE(output.find("100"), std::string::npos);
    EXPECT_NE(output.find("7"), std::string::npos);
    EXPECT_NE(output.find("3"), std::string::npos);
}
