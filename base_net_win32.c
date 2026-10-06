#if defined(_WIN32)
#include "base.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

B32 net_init(void)
{
    WSADATA wsa_data;

    return WSAStartup(MAKEWORD(2, 2), &wsa_data) == 0;
}

void net_shutdown(void)
{
    WSACleanup();
}

Socket net_socket_create(NetProtocol protocol)
{
    int type = (protocol == NetProtocol_TCP) ? SOCK_STREAM : SOCK_DGRAM;
    SOCKET s = socket(AF_INET, type, 0);

    return (Socket){(U64)s};
}

B32 net_socket_is_valid(Socket sock)
{
    // lt: INVALID_SOCKET is all-bits-set on win32, NOT 0 like a POSIX fd error.
    return (SOCKET)sock.handle != INVALID_SOCKET;
}

void net_socket_close(Socket sock)
{
    closesocket((SOCKET)sock.handle);
}

B32 net_socket_bind(Socket sock, U16 port)
{
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    return bind((SOCKET)sock.handle, (struct sockaddr*)&addr, sizeof(addr)) == 0;
}

B32 net_tcp_listen(Socket sock, I32 backlog)
{
    return listen((SOCKET)sock.handle, backlog) == 0;
}

Socket net_tcp_accept(Socket sock, NetAddr* out_addr)
{
    struct sockaddr_in addr = {0};
    int addr_len = sizeof(addr);
    SOCKET client = accept((SOCKET)sock.handle, (struct sockaddr*)&addr, &addr_len);
    if (out_addr != NULL) {
        out_addr->ip   = ntohl(addr.sin_addr.s_addr);
        out_addr->port = ntohs(addr.sin_port);
    }

    return (Socket){(U64)client};
}

B32 net_tcp_connect(Socket sock, NetAddr addr)
{
    struct sockaddr_in sa = {0};
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(addr.ip);
    sa.sin_port = htons(addr.port);

    return connect((SOCKET)sock.handle, (struct sockaddr*)&sa, sizeof(sa)) == 0;
}

I64 net_tcp_send(Socket sock, const void* data, U64 size)
{
    return (I64)send((SOCKET)sock.handle, (const char*)data, (int)size, 0);
}

I64 net_tcp_recv(Socket sock, void* buf, U64 size)
{
    return (I64)recv((SOCKET)sock.handle, (char*)buf, (int)size, 0);
}

I64 net_udp_sendto(Socket sock, const void* data, U64 size, NetAddr addr)
{
    struct sockaddr_in sa = {0};
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(addr.ip);
    sa.sin_port = htons(addr.port);

    return (I64)sendto((SOCKET)sock.handle, (const char*)data, (int)size, 0, (struct sockaddr*)&sa, sizeof(sa));
}

I64 net_udp_recvfrom(Socket sock, void* buf, U64 size, NetAddr* out_addr)
{
    struct sockaddr_in sa = {0};
    int addr_len = sizeof(sa);
    I64 n = (I64)recvfrom((SOCKET)sock.handle, (char*)buf, (int)size, 0,
                           (struct sockaddr*)&sa, &addr_len);
    if (out_addr != NULL) {
        out_addr->ip   = ntohl(sa.sin_addr.s_addr);
        out_addr->port = ntohs(sa.sin_port);
    }

    return n;
}

B32 net_resolve(String8 host, U16 port, NetAddr* out_addr)
{
    // lt: getaddrinfo needs a null-terminated C string; String8 isn't
    // guaranteed null-terminated, so copy it into a fixed stack buffer first.
    char host_cstr[256];
    U64 len = Min(host.length, sizeof(host_cstr) - 1);
    for (U64 i = 0; i < len; i += 1) {
        host_cstr[i] = (char)host.str[i];
    }
    host_cstr[len] = '\0';

    char port_cstr[8];
    wsprintfA(port_cstr, "%u", port);

    struct addrinfo hints = {0};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* result = NULL;
    if (getaddrinfo(host_cstr, port_cstr, &hints, &result) != 0) {
        return FALSE;
    }

    struct sockaddr_in* addr_in = (struct sockaddr_in*)result->ai_addr;
    out_addr->ip   = ntohl(addr_in->sin_addr.s_addr);
    out_addr->port = port;

    freeaddrinfo(result);
    return TRUE;
}

B32 net_socket_set_blocking(Socket sock, B32 blocking)
{
    // lt: ioctlsocket's FIONBIO takes an in/out u_long: 0 = blocking, nonzero = non-blocking.
    // We negate `blocking` so callers pass TRUE for "blocking", matching the function name intuitively.
    u_long mode = blocking ? 0 : 1;
    return ioctlsocket((SOCKET)sock.handle, FIONBIO, &mode) == 0;
}

B32 net_would_block(void)
{
    return WSAGetLastError() == WSAEWOULDBLOCK;
}

#endif // _WIN32

// lt: external declaration to prevent warning C4206 from MSVC (empty translation unit)
typedef int _compile;

