#include "Emulator.h"
#include <filesystem>

CPU cpu;

void cycle(void)
{
    cpu.cycle();
}
void cycle_ins_level(void)
{
    cpu.cycle_ins_level();
}

Emulator::Emulator()
    : m_clock(cycle), m_cpu(cpu), m_mem(cpu.mem)
{
    m_clock.setHZ(10000000);
}

int Emulator::init()
{
    m_cpu.init();
    m_clock.init();
    return 0;
}

int Emulator::load(std::string filename, std::string path)
{
    if(path.size() != 0)
    {
        filename = path + "/" + filename;
    }
    else
    {
        path = std::filesystem::path(filename).parent_path().string();
    }
    if(m_cpu.loadProgramFromFile(filename))
    {
        return 1;
    }
    return 0;
}

int Emulator::load(std::vector<uint8_t> &programData)
{
    return m_cpu.loadProgram(programData.data(), programData.size());
}

void Emulator::start()
{
    m_cpu.startExec();
    m_clock.setStatus(false);
}

void Emulator::stop()
{
    m_clock.terminate();
    m_cpu.stopPheripherials();
}

std::chrono::nanoseconds Emulator::getRunTime_ns()
{
    return m_clock.getRunTime_ns();
}