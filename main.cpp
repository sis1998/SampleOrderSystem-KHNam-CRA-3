#ifdef TESTING
#include <gmock/gmock.h>
int main(int argc, char** argv) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
#else
int main() {
    // application entry point
    return 0;
}
#endif
