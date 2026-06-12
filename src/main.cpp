#include <gtest/gtest.h>
#include "infra/ConsoleSetup.h"
#include "controller/AppController.h"

int main(int argc, char** argv) {
    // vstest passes --gtest_ flags; direct run has none
    bool testMode = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string_view(argv[i]).substr(0, 7) == "--gtest") {
            testMode = true; break;
        }
    }
    if (testMode) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }
    ConsoleSetup::apply();
    AppController app;
    app.run();
}
