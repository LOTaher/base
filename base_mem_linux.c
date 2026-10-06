#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)

#include "base.h"
#include <sys/mman.h>
#include <unistd.h>

void* mem_reserve(U64 size)
{
    void* ptr = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return (ptr == MAP_FAILED) ? NULL : ptr;
}

B32 mem_commit(void* ptr, U64 size)
{
    return mprotect(ptr, size, PROT_READ | PROT_WRITE) == 0;
}

void mem_decommit(void* ptr, U64 size)
{
    mprotect(ptr, size, PROT_NONE);
    madvise(ptr, size, MADV_DONTNEED);
}

void mem_release(void* ptr, U64 size)
{
    munmap(ptr, size);
}

U64 mem_page_size(void)
{
    return (U64)sysconf(_SC_PAGESIZE);
}

#endif // __linux__, __unix__, __APPLE__

// lt: external declaration to prevent warning C4206 from MSVC (empty translation unit)
typedef int _compile;

