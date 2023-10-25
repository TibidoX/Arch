#include <windows.h>
#include <iostream>
#include <filesystem>
#include "AppMonitor.h"

int main()
{
    Monitor m;
    if (!m.init()) {
        std::cerr << "Monitor initialization failed!\n";
        if (std::filesystem::exists(std::string("./resources/CREATED"))) {
            std::cerr << "Server initialization failed!\n";
        }
        return 1;
    }// in practice some sort of alarm should be raised here

    Sleep(3000);

    while (1) {
        if (!m.check()) {
            Monitor::getAndSetPort();
            Monitor::deleteResource();
            Monitor::reset();
            m.init();
        }
        Sleep(3000);
    }
}