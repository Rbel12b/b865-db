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
    std::chrono::nanoseconds getRunTime_ns();
    void start();
    void stop();

private:

private:
    Clock m_clock;
    CPU &m_cpu;
    MEMORY &m_mem;
};

#endif // _COMPUTER_H_