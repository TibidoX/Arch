#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <string>
#include <filesystem>
#include <process.h>
#include "AppMonitor.h"
#include "helpers/Process.h"
#include "../helpers/UtilString.h"
#include "../helpers/UtilFile.h"

static Process sServer;
static Process sSpareServer;
static std::string sPort = "0";

bool Monitor::initMainServer() {
    m_console.handleCtrlC(Monitor::exit);
    char cmd[256] = {};
    sprintf(cmd, "Server.exe %s", sPort.c_str());
    bool ok = sServer.create(cmd);  // launching Server
    printf(ok ? "monitoring \"%s\"\n" : "error: cannot monitor \"%s\"\n", cmd);
    return ok;
}

bool Monitor::initSpareServer() {
    m_console.handleCtrlC(Monitor::exit);
    char cmd[256] = {};
    sprintf(cmd, "Server.exe %s", sPort.c_str());
    bool ok = sSpareServer.create(cmd);  // launching Server
    printf(ok ? "spare pid = %s\n": "error: cannot monitor spare pid = %s\n", sSpareServer.pid().c_str());

    //fileWriteStr(std::string("resources\\STATE_SPARE"), "");
    char* state = fileReadStr("resources\\STATE"); // load state from previous run
    if (state)
    {
        for (std::string& line : split(state, "\n"))
            if (!line.empty()) {
                fileAppend(std::string("resources\\STATE_SPARE"), line+"\n");
            }
        delete[] state;
    }

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
    std::string directoryPath = ".\\resources";
    for (const auto& file : std::filesystem::directory_iterator(directoryPath)) {
        if (std::filesystem::is_regular_file(file)) {
            if (file.path().filename() != "STATE") {
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
    if (std::filesystem::exists("resources\\STATE_SPARE")) {
        std::filesystem::remove("resources\\STATE");
        std::filesystem::rename("resources\\STATE_SPARE", "resources\\STATE");
    }
    sServer.resume();
}

void Monitor::exit() {
    sServer.terminate();
    sSpareServer.terminate();
}