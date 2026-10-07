#include "common.h"
#include "tests.h"
#include "ui.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char** argv) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    if (argc > 1 && std::string(argv[1]) == "--test") {
        runAllTests();
        return 0;
    }
    mainMenu();
    return 0;
}