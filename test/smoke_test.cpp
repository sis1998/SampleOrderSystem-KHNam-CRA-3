// Smoke test: verifies that the Google Test framework is correctly configured.
// This test always passes and confirms the build + link environment is functional.
#include <gtest/gtest.h>

// Always-pass smoke test -- confirms the test runner itself is operational.
TEST(SmokeTest, AlwaysPass)
{
    EXPECT_TRUE(true);
}
