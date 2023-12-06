#include <windows.h>
#include <filesystem>
#include <iostream>
#include "AppMonitor.h"

int main() {
    Monitor m;

    /** Free all files before start up except for STATE */
    m.freeResourceDir();

    if (!m.initMainServer()) {
        std::cerr << "Monitor initialization failed!\n";
        if (std::filesystem::exists(std::string("./resources/CREATED"))) {
            std::cerr << "Server initialization failed!\n";
        }
        return 1;
    }

    Sleep(1000);
    m.getAndSetGlobalPort();
    m.initSpareServer();

    while (1) {
        if (!m.isServerAlive()) {
            m.changeSpareToMain();
            m.freeResourceDir();
            m.initSpareServer();
        }
        Sleep(1000);
    }
}