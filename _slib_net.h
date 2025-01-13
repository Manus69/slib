#ifndef _SLIB_NET_H
#define _SLIB_NET_H

#include "_slib.h"

#include <arpa/inet.h>

typedef struct Conn Conn;

struct Conn
{
    struct sockaddr_in  addr;
    int                 socket;
};

bool    Conn_init_sender(Conn * conn, const char * ipcstr, int port);
bool    Conn_init_recvr(Conn * conn, int port);
void    Conn_close(Conn * conn);
i32     Conn_send(const Conn * conn, const void * msg, i32 len);
i32     Conn_recv(const Conn * conn, void * buff, i32 len);
bool    Conn_has_data(const Conn * conn);

#ifdef _SLIB_NET_IMPL

#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

bool Conn_init_sender(Conn * conn, const char * ipcstr, int port)
{
    * conn = (Conn) {};

    conn->addr.sin_family = AF_INET;
    conn->addr.sin_port = htons(port);

    if ((conn->socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) return false;
    if (inet_pton(AF_INET, ipcstr, & (conn->addr.sin_addr)) <= 0) return false;

    return true;
}

bool Conn_init_recvr(Conn * conn, int port)
{
    * conn = (Conn) {};

    conn->addr.sin_family = AF_INET;
    conn->addr.sin_port = htons(port);
    conn->addr.sin_addr.s_addr = INADDR_ANY;

    if ((conn->socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) return false;
    if (bind(conn->socket, (struct sockaddr *) & conn->addr, sizeof(conn->addr))) return false;

    return true;
}

void Conn_close(Conn * conn)
{
    close(conn->socket);
}

i32 Conn_send(const Conn * conn, const void * msg, i32 len)
{
    return sendto(conn->socket, msg, len, 0, (struct sockaddr *) & conn->addr, sizeof(conn->addr));
}

i32 Conn_recv(const Conn * conn, void * buff, i32 len)
{
    return recvfrom(conn->socket, buff, len, 0, 0, 0);
}

bool Conn_has_data(const Conn * conn)
{
    struct pollfd pfd;

    pfd = (struct pollfd)
    {
        .fd = conn->socket,
        .events = POLLIN,
    };

    return poll(& pfd, 1, 0) > 0;
}

#endif
#endif