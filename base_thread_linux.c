#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#include "base.h"
#include <pthread.h>
#include <stdlib.h>

// lt: pthreads requires a start routine shaped void*(*)(void*),
// but ThreadFunc is void(*)(void*) with no return value. Same trampoline
// pattern as the win32 side bridges the two signatures.
typedef struct {
    ThreadFunc func;
    void* arg;
} ThreadStartData;

internal void* thread_trampoline(void* param)
{
    ThreadStartData* data = (ThreadStartData*)param;
    ThreadFunc func = data->func;
    void* arg = data->arg;
    free(data);

    func(arg);

    return NULL;
}

Thread thread_create(ThreadFunc func, void* arg)
{
    ThreadStartData* data = (ThreadStartData*)malloc(sizeof(ThreadStartData));
    if (data == NULL) {
        return (Thread){0};
    }
    data->func = func;
    data->arg = arg;

    pthread_t tid;
    if (pthread_create(&tid, NULL, thread_trampoline, data) != 0) {
        free(data);
        return (Thread){0};
    }

    // lt: pthread_t is an unsigned long on Linux/glibc, so this cast
    // is safe here. Not guaranteed portable to every POSIX platform (some
    // implementations use an opaque struct for pthread_t), but fine for our
    // Linux-only target.
    return (Thread){(U64)tid};
}

void thread_join(Thread thread)
{
    pthread_t tid = (pthread_t)thread.handle;
    pthread_join(tid, NULL);
}

#endif // __linux__, __unix__, __APPLE__

// lt: external declaration to prevent warning C4206 from MSVC (empty translation unit)
typedef int _compile;
