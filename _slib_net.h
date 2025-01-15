#ifndef _SLIB_NET_H
#define _SLIB_NET_H

#include "_slib.h"

#include <arpa/inet.h>

typedef struct Trans Trans;

struct Trans
{
    struct sockaddr_in  addr;
    int                 socket;
};

bool    Trans_init_sender(Trans * trans, const char * ipcstr, int port);
bool    Trans_init_recvr(Trans * trans, int port);
void    Trans_close(Trans * trans);
i32     Trans_send(const Trans * trans, const void * msg, i32 len);
i32     Trans_recv(const Trans * trans, void * buff, i32 len);
bool    Trans_has_data(const Trans * trans);

#ifdef _SLIB_NET_IMPL

#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

bool Trans_init_sender(Trans * trans, const char * ipcstr, int port)
{
    * trans = (Trans) {};

    trans->addr.sin_family = AF_INET;
    trans->addr.sin_port = htons(port);

    if ((trans->socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) return false;
    if (inet_pton(AF_INET, ipcstr, & (trans->addr.sin_addr)) <= 0) return false;

    return true;
}

bool Trans_init_recvr(Trans * trans, int port)
{
    * trans = (Trans) {};

    trans->addr.sin_family = AF_INET;
    trans->addr.sin_port = htons(port);
    trans->addr.sin_addr.s_addr = INADDR_ANY;

    if ((trans->socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) return false;
    if (bind(trans->socket, (struct sockaddr *) & trans->addr, sizeof(trans->addr))) return false;

    return true;
}

void Trans_close(Trans * trans)
{
    close(trans->socket);
}

i32 Trans_send(const Trans * trans, const void * msg, i32 len)
{
    return sendto(trans->socket, msg, len, 0, (struct sockaddr *) & trans->addr, sizeof(trans->addr));
}

i32 Trans_recv(const Trans * trans, void * buff, i32 len)
{
    struct sockaddr_in  addr;
    socklen_t           addr_len;
    i32                 recieved;

    addr_len = sizeof(trans->addr);
    recieved = recvfrom(trans->socket, buff, len, 0, (struct sockaddr *) & addr, & addr_len);
    if (addr.sin_addr.s_addr != trans->addr.sin_addr.s_addr) return NO_IDX;

    return recieved;
}

bool Trans_has_data(const Trans * trans)
{
    struct pollfd pfd;

    pfd = (struct pollfd)
    {
        .fd = trans->socket,
        .events = POLLIN,
    };

    return poll(& pfd, 1, 0) > 0;
}

#endif
#endif