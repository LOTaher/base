#if defined(_WIN32)

#include "base.h"
#include <windows.h>

void* mem_reserve(U64 size)
{
    return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
}

B32 mem_commit(void* ptr, U64 size)
{
    return VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != NULL;
}

void mem_decommit(void* ptr, U64 size)
{
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

void mem_release(void* ptr, U64 size)
{
    (void)size; // lt: unused on Windows but would like to keep signatures the same
    VirtualFree(ptr, 0, MEM_RELEASE);
}

U64 mem_page_size(void)
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwPageSize;
}

#endif // _WIN32

// lt: external declaration to prevent warning C4206 from MSVC (empty translation unit)
typedef int _compile;

