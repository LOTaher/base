#include "base_arena.c"
#include "base_string.c"
#include "base_net.c"

// lt: platform variants self-guard with #if defined(...)/#endif,
// so both sides can be included unconditionally; only one compiles in.
#include "base_mem_linux.c"
#include "base_mem_win32.c"
#include "base_net_linux.c"
#include "base_net_win32.c"
#include "base_thread_linux.c"
#include "base_thread_win32.c"
#include "base_time_linux.c"
#include "base_time_win32.c"
#include "base_math.c"
