// Google Test entry point.
// gtest-all.cc and gmock-all.cc are compiled by gmock.targets,
// so only InitGoogleMock and RUN_ALL_TESTS are needed here.
#include <gmock/gmock.h>

int main(int argc, char* argv[])
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
