#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <string>
#include <filesystem>
#include "AppMonitor.h"
#include "helpers/Process.h"
#include "../helpers/UtilString.h"
#include "../helpers/UtilFile.h"

static Process sServer;
static std::string sPort = "0";

bool Monitor::init()
{
    m_console.handleCtrlC(Monitor::reset); // if Monitor's execution is aborted via Ctrl+C, reset() cleans up its internal state
    char cmd[256] = {};
    sprintf(cmd, "Server.exe %s", sPort.c_str());
    bool ok = sServer.create(cmd); // launching Server
    printf(ok ? "monitoring \"%s\"\n" : "error: cannot monitor \"%s\"\n", cmd);
    return ok;
}

bool Monitor::check()
{
    std::string heartBeatFilePath = std::string("./resources/ALIVE" + sServer.pid());

    // If we got heartbeat from Server as a file
    // Remove this file, and wait for another beat
    bool isGotBeat = fileExists(heartBeatFilePath);
    std::filesystem::remove(heartBeatFilePath);

    // Check if we got beat and if Server process didn't crash
    if (!isGotBeat || !sServer.wait(3000)) {
        return false;
    }
    return true;
}

void Monitor::deleteResource() {
    std::string path = "./resources";
    for (const auto& file : std::filesystem::directory_iterator(path)) {
        if (std::filesystem::is_regular_file(file)) {
            if (file.path().filename() != "./STATE") {
                std::filesystem::remove_all(file.path());
            }
        }
    }
}

void Monitor::getAndSetPort() {
    std::string path = std::string("./resources/CREATED");
    sPort = split(fileReadStr(path), ",")[0];
}

void Monitor::reset()
{
    sServer.terminate();
}