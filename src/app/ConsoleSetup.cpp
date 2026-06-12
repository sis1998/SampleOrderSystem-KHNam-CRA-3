#include "ConsoleSetup.h"
#include <windows.h>

namespace oss::app {

void ConsoleSetup::initialize() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

} // namespace oss::app
