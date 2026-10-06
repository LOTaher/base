#if defined(_WIN32)

#include "base.h"
#include <windows.h>

U64 time_now_ms(void)
{
    return (U64)GetTickCount64();
}

void time_sleep_ms(U64 ms)
{
    Sleep((DWORD)ms);
}

#endif // _WIN32

// lt: external declaration to prevent warning C4206 from MSVC (empty translation unit)
typedef int _compile;

