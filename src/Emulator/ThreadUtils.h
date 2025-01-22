#pragma once
#ifndef _B865_EMU_UTIL_H
#define _B865_EMU_UTIL_H

#include <string>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <sched.h>
#endif

void pinThreadToCore(std::thread::native_handle_type handle, int core_id);
void setThreadPriority(std::thread::native_handle_type handle, bool high_priority);

#endif