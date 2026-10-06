#ifndef BASE_H
#define BASE_H

#include <stdint.h>

////////////////////////////////
//~ lt: Base Layer

typedef int8_t   I8;
typedef int16_t  I16;
typedef int32_t  I32;
typedef int64_t  I64;
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef I8       B8;
typedef I32      B32;
typedef float    F32;
typedef double   F64;

#define Min(a, b) \
    ((a) < (b) ? (a) : (b))
#define Max(a, b) \
    ((a) > (b) ? (a) : (b))

// lt: don't let A go above X
#define ClampTop(A,X) Min(A,X)
// lt: don't let B go below X
#define ClampBot(B,X) Max(B,X)
#define Clamp(A,X,B) (((X)<(A))?(A):((X)>(B))?(B):(X))

#define KiB(n) \
    ((U64)(n) << 10)
#define MiB(n) \
    ((U64)(n) << 20)
#define GiB(n) \
    ((U64)(n) << 30)

#define ArrayLength(arr) \
    (sizeof(arr) / sizeof((arr)[0]))

#define local static
#define global static
#define internal static

#define TRUE 1
#define FALSE 0

////////////////////////////////
//~ lt: Assert

#if defined(_MSC_VER) && !defined(__clang__)
# define Trap() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
# define Trap() __builtin_trap()
#else
# error UNSUPPORTED COMPILER
#endif

#define AssertAlways(x) do{if(!(x)) {Trap();}}while(0)

#if BUILD_DEBUG
# define Assert(x) AssertAlways(x)
#else
# define Assert(x) (void)(x)
#endif

////////////////////////////////
//~ lt: Memory Library

extern void* mem_reserve(U64 size);               // Reserve address space without physical commits.
extern B32   mem_commit(void* ptr, U64 size);     // Commit pages and map to physical memory.
extern void  mem_decommit(void* ptr, U64 size);   // Decommit pages.
extern void  mem_release(void* ptr, U64 size);    // Release the reservation.
extern U64   mem_page_size(void);                 // Check the page size of the system.

////////////////////////////////
//~ lt: Arena Library

typedef struct {
    U64 capacity;   // Reserved address space
    U64 committed;  // Backed by physical memory
    U64 pos;        // Current allocation offset
} Arena;

typedef struct {
    Arena* arena;
    U64 pos;
} ArenaTemp;

#define ARENA_COMMIT_CHUNK MiB(1) // How much memory to commit at a time

extern Arena*     arena_create(U64 capacity);                    // Create an Arena Allocator.
extern void       arena_destroy(Arena* arena);                   // Destroy (free) an Arena.
extern U64        arena_align_forward(U64 pos, U64 alignment);   // Align memory to the next power of 2.
extern void*      arena_push(Arena* arena, U64 size);            // Push new memory onto the Arena.
extern void       arena_clear(Arena* arena);                     // Clear the memory in an Arena.
extern U64        arena_mark(Arena* arena);                      // Mark the current position in an Arena.
extern void       arena_pop(Arena* arena, U64 mark);             // Pop to the memory mark of an Arena.

extern ArenaTemp  arena_temp_begin(Arena* arena);                // Create a Temp_Arena.
extern void       arena_temp_end(ArenaTemp arena);               // Destory a Temp_Arena.

////////////////////////////////
//~ lt: String Library

typedef struct {
  U8* str;
  U64 length;
} String8;

#define string_lit(s) \
    (String8){(U8 *)(s), sizeof(s) - 1}
#define string_fmt(s) \
    (int)(s).length, (s).str

extern String8 string_substring(String8 str, U64 start, U64 end);       // Get the substring of a given string.
extern String8 string_cstring(char *str);                               // Convert a C string to a String8.
extern String8 string_copy(String8 str, Arena *arena);                  // Copy a new string in memory.
extern String8 string_concat(String8 str1, String8 str2, Arena *arena); // Concat two strings together.
extern String8 string_create_fmt(Arena *arena, const char *fmt, ...);   // Create a formatted (printf-style) string.
extern B8      string_compare(String8 str1, String8 str2);              // Check whether two strings are equal.
extern B8      string_contains(String8 str, String8 substr);            // Check whether a string contains another.

////////////////////////////////
//~ lt: Net Library

typedef struct {
    U64 handle; // lt: SOCKET on win32, int fd on linux. it'll be casted inside net_*.c
} Socket;

typedef struct {
    U32 ip;
    U16 port;
} NetAddr;

typedef enum {
    NetProtocol_UDP,
    NetProtocol_TCP,
} NetProtocol;

#define NET_INVALID_SOCKET ((Socket){0})

extern B32      net_init(void);                                                              // Platform net init (WSAStartup on win32, nothing on linux)
extern void     net_shutdown(void);                                                          // Platform net shutdown (WSACleanup on win32, nothing on linux)
extern Socket   net_socket_create(NetProtocol protocol);                                     // Create a socket of the given protocol.
extern B32      net_socket_is_valid(Socket sock);                                            // Check whether a socket handle is valid.
extern void     net_socket_close(Socket sock);                                               // Close socket
extern B32      net_socket_bind(Socket sock, U16 port);                                      // Bind a socket to a local port.
extern B32      net_socket_set_blocking(Socket sock, B32 blocking);                          // Set a socket's blocking mode
extern B32      net_would_block(void);                                                       // Check whether the last failed net call failed only because no data was ready.
extern B32      net_resolve(String8 host, U16 port, NetAddr* out_addr);                      // Resolve a hostname/IP string to a NetAddr.
extern B32      net_tcp_listen(Socket sock, I32 backlog);                                    // (TCP) Mark socket as listening.
extern Socket   net_tcp_accept(Socket sock, NetAddr* out_addr);                              // (TCP) Accept an incoming connection.
extern B32      net_tcp_connect(Socket sock, NetAddr addr);                                  // (TCP) Connect to a remote address.
extern I64      net_tcp_send(Socket sock, const void* data, U64 size);                       // (TCP) Send on a connected socket.
extern B32      net_tcp_send_exact(Socket sock, void* buf, U64 send_size);                   // (TCP) Send a specific number of bytes to a connected socket.
extern I64      net_tcp_recv(Socket sock, void* buf, U64 size);                              // (TCP) Receive on a connected socket.
extern B32      net_tcp_recv_exact(Socket sock, void* buf, U64 buffer_size, U64 recv_size);  // (TCP) Recieves a specific number of bytes from a connected socket.
extern I64      net_udp_sendto(Socket sock, const void* data, U64 size, NetAddr addr);       // (UDP) Send to an address.
extern I64      net_udp_recvfrom(Socket sock, void* buf, U64 size, NetAddr* out_addr);       // (UDP) Receive, capturing sender address.

////////////////////////////////
//~ lt: Time Library

extern U64   time_now_ms(void);               // Monotonic ms for measuring elapsed time.
extern void  time_sleep_ms(U64 ms);           // Pause calling for a number of milliseconds.

////////////////////////////////
//~ lt: Thread Library

typedef struct {
    U64 handle; // lt: HANDLE (cast from void*) on wind32, pthread_t on linux
} Thread;

typedef void (*ThreadFunc)(void* arg);

extern Thread thread_create(ThreadFunc func, void* arg);   // Create and start a new thread that is running func(arg).
extern void   thread_join(Thread thread);                  // Block until the thread finishes.

////////////////////////////////
//~ lt: Math Library

// typedef struct {
//     F32 x;
//     F32 y;
// } Vector2D;
//
// typedef struct {
//     F32 x;
//     F32 y;
//     F32 z;
// } Vector3D;

#endif // BASE_H
