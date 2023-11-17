#pragma once
#include "helpers/Console.h"

// Monitor app that manages Server's lifecycle and monitors its state
class Monitor
{
public:
    Monitor() = default;
    virtual ~Monitor() = default;
    bool init(); // launches Server
    bool initSpare(); // launches Spare
    bool activateSpare();
    void resetSpare();
    bool check(); // checks Server state
    void reset(); // terminates irresponsive Server

    static void deleteResource();
    static void getAndSetPort();

private:
    Console m_console;
};
