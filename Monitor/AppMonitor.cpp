#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <string>
#include <filesystem>
#include "AppMonitor.h"
#include "helpers/Process.h"
#include "../helpers/UtilString.h"
#include "../helpers/UtilFile.h"

static Process sServer;
static Process sSpareServer;
static std::string sPort = "0";

bool Monitor::initMainServer() {
    char cmd[256] = {};
    sprintf(cmd, "Server.exe %s", sPort.c_str());
    bool ok = sServer.create(cmd);  // launching Server
    printf(ok ? "monitoring \"%s\"\n" : "error: cannot monitor \"%s\"\n", cmd);
    return ok;
}

bool Monitor::initSpareServer() {
    char cmd[256] = {};
    sprintf(cmd, "Server.exe %s", sPort.c_str());
    bool ok = sSpareServer.create(cmd);  // launching Server
    printf(ok ? "monitoring spare pid = %s\n": "error: cannot monitor spare pid = %s\n", sSpareServer.pid().c_str());
    sSpareServer.suspend();
    return ok;
}

bool Monitor::isServerAlive() {
    std::string heartBeatFilePath = std::string("./resources/ALIVE" + sServer.pid());

    bool isGotBeat = fileExists(heartBeatFilePath);
    std::filesystem::remove(heartBeatFilePath);

    if (!isGotBeat || !sServer.wait(3000)) {
        return false;
    }
    return true;
}

void Monitor::reset() {
    sServer.terminate();
}

void Monitor::freeResourceDir() {
    std::string directoryPath = "./resources";
    for (const auto& file : std::filesystem::directory_iterator(directoryPath)) {
        if (std::filesystem::is_regular_file(file)) {
            if (file.path().filename() != ".\\STATE") {
                std::filesystem::remove_all(file.path());
            }
        }
    }
}

void Monitor::getAndSetGlobalPort() {
    std::string path = std::string("./resources/CREATED");
    sPort = split(fileReadStr(path), ",")[0];
}

void Monitor::changeSpareToMain() {
    sServer.terminate();
    sServer = sSpareServer;
    sServer.resume();
}