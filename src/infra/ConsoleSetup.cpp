#include "ConsoleSetup.h"
#include <windows.h>

void ConsoleSetup::apply() {
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
}
