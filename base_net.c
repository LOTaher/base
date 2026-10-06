#include "base.h"

B32 net_tcp_recv_exact(Socket sock, void* buf, U64 buffer_size, U64 recv_size)
{
    if (recv_size > buffer_size) {
        return FALSE;
    }

    U64 total_read = 0;
    U8* bytes = (U8*)buf;

    while (total_read < recv_size) {
        I64 n = net_tcp_recv(sock, bytes + total_read, recv_size - total_read);
        if (n <= 0) {
            return FALSE;
        }
        total_read += (U64)n;
    }

    return TRUE;
}

B32 net_tcp_send_exact(Socket sock, void* buf, U64 send_size)
{
    U64 total_sent = 0;
    U8* bytes = (U8*)buf;

    while (total_sent < send_size) {
        I64 n = net_tcp_send(sock, bytes + total_sent, send_size - total_sent);
        if (n <= 0) {
            return FALSE;
        }
        total_sent += (U64)n;
    }

    return TRUE;
}
