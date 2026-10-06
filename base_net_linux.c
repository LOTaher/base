#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)

#include "base.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

B32 net_init(void)
{
    // lt: no global init step needed on POSIX, unlike WSAStartup on win32.
    return TRUE;
}

void net_shutdown(void)
{
    // lt: no global shutdown step needed on POSIX, unlike WSACleanup on win32.
}

Socket net_socket_create(NetProtocol protocol)
{
    int type = (protocol == NetProtocol_TCP) ? SOCK_STREAM : SOCK_DGRAM;
    int fd = socket(AF_INET, type, 0);

    return (Socket){(U64)fd};
}

B32 net_socket_is_valid(Socket sock)
{
    // lt: a failed socket() returns -1 on POSIX, NOT all-bits-set like win32.
    return (I64)sock.handle >= 0;
}

void net_socket_close(Socket sock)
{
    close((int)sock.handle);
}

B32 net_socket_bind(Socket sock, U16 port)
{
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    return bind((int)sock.handle, (struct sockaddr*)&addr, sizeof(addr)) == 0;
}

B32 net_tcp_listen(Socket sock, I32 backlog)
{
    return listen((int)sock.handle, backlog) == 0;
}

Socket net_tcp_accept(Socket sock, NetAddr* out_addr)
{
    struct sockaddr_in addr = {0};
    socklen_t addr_len = sizeof(addr);
    int client_fd = accept((int)sock.handle, (struct sockaddr*)&addr, &addr_len);
    if (out_addr != NULL) {
        out_addr->ip   = ntohl(addr.sin_addr.s_addr);
        out_addr->port = ntohs(addr.sin_port);
    }

    return (Socket){(U64)client_fd};
}

B32 net_tcp_connect(Socket sock, NetAddr addr)
{
    struct sockaddr_in sa = {0};
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(addr.ip);
    sa.sin_port = htons(addr.port);

    return connect((int)sock.handle, (struct sockaddr*)&sa, sizeof(sa)) == 0;
}

I64 net_tcp_send(Socket sock, const void* data, U64 size)
{
    return (I64)send((int)sock.handle, data, size, 0);
}

I64 net_tcp_recv(Socket sock, void* buf, U64 size)
{
    return (I64)recv((int)sock.handle, buf, size, 0);
}

I64 net_udp_sendto(Socket sock, const void* data, U64 size, NetAddr addr)
{
    struct sockaddr_in sa = {0};
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(addr.ip);
    sa.sin_port = htons(addr.port);

    return (I64)sendto((int)sock.handle, data, size, 0, (struct sockaddr*)&sa, sizeof(sa));
}

I64 net_udp_recvfrom(Socket sock, void* buf, U64 size, NetAddr* out_addr)
{
    struct sockaddr_in sa = {0};
    socklen_t addr_len = sizeof(sa);
    I64 n = (I64)recvfrom((int)sock.handle, buf, size, 0, (struct sockaddr*)&sa, &addr_len);
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
    snprintf(port_cstr, sizeof(port_cstr), "%u", port);

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
    int fd = (int)sock.handle;
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        return FALSE;
    }
    // lt: O_NONBLOCK is the inverse of "blocking", so we set/clear it based on the input.
    if (blocking) {
        flags &= ~O_NONBLOCK;
    } else {
        flags |= O_NONBLOCK;
    }
    return fcntl(fd, F_SETFL, flags) == 0;
}

B32 net_would_block(void)
{
    // lt: errno is must be called after a failing net_udp_recvfrom since its a global
    return errno == EAGAIN || errno == EWOULDBLOCK;
}

#endif // __linux__, __unix__, __APPLE__

// lt: external declaration to prevent warning C4206 from MSVC (empty translation unit)
typedef int _compile;

