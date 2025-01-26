#pragma once
#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include <string>
#include <vector>
#include <chrono>
#include <cstdint>
#include "CPU.h"
#include "Clock.h"

void cycle();

class Emulator
{
public:
    Emulator();
    int init();
    int load(std::string filename, std::string path = "");
    int load(std::vector<uint8_t> &programData);
    void setBreakpoints(std::vector<uint16_t>& breakpoints);
    std::chrono::nanoseconds getRunTime_ns();
    void start();
    void stop();
    void terminate();
    bool paused();
    bool clockRunning();
    void continue_exec();

public:
    CPU &m_cpu;

private:

private:
    Clock m_clock;
    MEMORY &m_mem;
};

#endif // _COMPUTER_H_