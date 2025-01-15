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

typedef struct TransRecv TransRecv;

struct TransRecv
{
    struct in_addr  addr;
    i32             len;
};

typedef struct Tunnel Tunnel;

struct Tunnel
{
    Trans   recvr;
    Trans   sender;
};


bool        Trans_init_sender(Trans * trans, const char * ipcstr, int port);
bool        Trans_init_recvr(Trans * trans, int port);
void        Trans_close(Trans * trans);
i32         Trans_send(const Trans * trans, const void * msg, i32 size);
TransRecv   Trans_recv(const Trans * trans, void * buff, i32 size);
bool        Trans_has_data(const Trans * trans);

bool        Tunnel_init(Tunnel * tun, const char * ipcstr, int my_port, int his_port);
void        Tunnel_close(Tunnel * tun);
bool        Tunnel_send(const Tunnel * tun, const void * msg, i32 size);
i32         Tunnel_recv_wait(const Tunnel * tun, void * buff, i32 size);
i32         Tunnel_recv(const Tunnel * tun, void * buff, i32 size);


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

i32 Trans_send(const Trans * trans, const void * msg, i32 size)
{
    return sendto(trans->socket, msg, size, 0, (struct sockaddr *) & trans->addr, sizeof(trans->addr));
}

TransRecv Trans_recv(const Trans * trans, void * buff, i32 size)
{
    struct sockaddr_in  addr;
    socklen_t           addr_len;
    i32                 recieved;

    addr_len = sizeof(trans->addr);
    recieved = recvfrom(trans->socket, buff, size, 0, (struct sockaddr *) & addr, & addr_len);

    return (TransRecv)
    {
        .addr = addr.sin_addr,
        .len = recieved,
    };
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

bool Tunnel_init(Tunnel * tun, const char * ipcstr, int my_port, int his_port)
{
    if (! Trans_init_recvr(& tun->recvr, my_port)) return false;
    if (! Trans_init_sender(& tun->sender, ipcstr, his_port)) return false;


    return true;
}

void Tunnel_close(Tunnel * tun)
{
    Trans_close(& tun->sender);
    Trans_close(& tun->recvr);
}

bool Tunnel_send(const Tunnel * tun, const void * msg, i32 size)
{
    i32 len;

    len = Trans_send(& tun->sender, msg, size);
    
    return len == size;
}

i32 Tunnel_recv_wait(const Tunnel * tun, void * buff, i32 size)
{
    TransRecv tr;

    tr = Trans_recv(& tun->recvr, buff, size);
    if (tr.addr.s_addr == tun->sender.addr.sin_addr.s_addr) return tr.len;

    return NO_IDX;
}

i32 Tunnel_recv(const Tunnel * tun, void * buff, i32 size)
{
    if (! Trans_has_data(& tun->recvr)) return 0;

    return Tunnel_recv_wait(tun, buff, size);
}

#endif
#endif