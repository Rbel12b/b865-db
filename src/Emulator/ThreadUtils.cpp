#include "ThreadUtils.h"
void pinThreadToCore(std::thread::native_handle_type handle, int core_id)
{
#ifdef _WIN32

    DWORD_PTR mask = (1ULL << core_id);
    if (!SetThreadAffinityMask((HANDLE)handle, mask))
    {
        fprintf(stderr, "Failed to set thread affinity. Error: %lu\n", GetLastError());
    }
#else
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);

    if (pthread_setaffinity_np(handle, sizeof(cpu_set_t), &cpuset) != 0)
    {
        fprintf(stderr, "Failed to set thread affinity.\n");
    }
#endif
}

void setThreadPriority(std::thread::native_handle_type handle, bool high_priority)
{
#ifdef _WIN32
    int priority = high_priority ? THREAD_PRIORITY_HIGHEST : THREAD_PRIORITY_NORMAL;
    if (!SetThreadPriority((HANDLE)handle, priority))
    {
        fprintf(stderr, "Failed to set thread priority. Error: %lu\n", GetLastError());
    }
#else
    sched_param sch_params;
    sch_params.sched_priority = high_priority ? sched_get_priority_max(SCHED_FIFO) : 0;

    int result = pthread_setschedparam(handle, high_priority ? SCHED_FIFO : SCHED_OTHER, &sch_params) != 0;
    if (result != 0)
    {
        if (errno == EPERM) {
            fprintf(stderr, "Failed to set thread priority: Insufficient permissions.\n");
        } else if (errno == EINVAL) {
            fprintf(stderr, "Failed to set thread priority: Invalid policy or priority.\n");
        } else {
            fprintf(stderr, "Failed to set thread priority. Error code: %d\n", result);
        }
    }
#endif
}