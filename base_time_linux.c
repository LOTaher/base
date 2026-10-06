#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)

#include "base.h"
#include <time.h>

U64 time_now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (U64)ts.tv_sec * 1000 + (U64)ts.tv_nsec / 1000000;
}

void time_sleep_ms(U64 ms)
{
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

#endif // __linux__, __unix__, __APPLE__

// lt: external declaration to prevent warning C4206 from MSVC (empty translation unit)
typedef int _compile;

