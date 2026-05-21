/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_SOCKET_H
#define DICE_SOCKET_H

#include <sys/socket.h>

#define EVENT_ACCEPT      104
#define EVENT_ACCEPT4     105
#define EVENT_BIND        106
#define EVENT_CONNECT     107
#define EVENT_GETPEERNAME 108
#define EVENT_GETSOCKNAME 109
#define EVENT_SETSOCKOPT  110
#define EVENT_LISTEN      111
#define EVENT_RECV        112
#define EVENT_RECVFROM    113
#define EVENT_RECVMSG     114
#define EVENT_SEND        115
#define EVENT_SENDTO      116
#define EVENT_SENDMSG     117
#define EVENT_SHUTDOWN    118
#define EVENT_SOCKET      119
#define EVENT_SOCKETPAIR  120

struct accept_event {
    const void *pc;
    int sockfd;
    struct sockaddr* addr;
    socklen_t* addrlen;
    int ret;
    int(*func)(int, struct sockaddr*, socklen_t*);
};

struct accept4_event {
    const void *pc;
    int sockfd;
    struct sockaddr* addr;
    socklen_t* addrlen;
    int flags;
    int ret;
    int(*func)(int, struct sockaddr*, socklen_t*, int);
};

struct bind_event {
    const void *pc;
    int sockfd;
    const struct sockaddr *addr;
    socklen_t addrlen;
    int ret;
    int(*func)(int, const struct sockaddr*, socklen_t);
};

struct connect_event {
    const void *pc;
    int sockfd;
    struct sockaddr *addr;
    socklen_t addrlen;
    int ret;
    int(*func)(int, const struct sockaddr*, socklen_t);
};

struct getpeername_event {
    const void *pc;
    int sockfd;
    struct sockaddr *addr;
    socklen_t *addrlen;
    int ret;
    int(*func)(int, struct sockaddr*, socklen_t*);
};

struct getsockname_event {
    const void *pc;
    int sockfd;
    struct sockaddr *addr;
    socklen_t *addrlen;
    int ret;
    int(*func)(int, struct sockaddr*, socklen_t*);
};

struct setsockopt_event {
    const void *pc;
    int sockfd;
    int level;
    int optname;
    const void *optval;
    socklen_t optlen;
    int ret;
    int(*func)(int, int, int, const void*, socklen_t);
};

struct listen_event {
    const void *pc;
    int sockfd;
    int backlog;
    int ret;
    int(*func)(int, int);
};

struct recv_event {
    const void *pc;
    int sockfd;
    void *buf;
    size_t len;
    int flags;
    ssize_t ret;
    ssize_t(*func)(int, void*, size_t, int);
};

struct recvfrom_event {
    const void *pc;
    int sockfd;
    void *buf;
    size_t len;
    int flags;
    struct sockaddr *src_addr;
    socklen_t *addrlen;
    ssize_t ret;
    ssize_t(*func)(int, void*, size_t, int, struct sockaddr*, socklen_t*);
};

struct recvmsg_event {
    const void *pc;
    int sockfd;
    struct msghdr *msg;
    int flags;
    ssize_t ret;
    ssize_t(*func)(int, struct msghdr*, int);
};

struct send_event {
    const void *pc;
    int sockfd;
    const void *buf;
    size_t len;
    int flags;
    ssize_t ret;
    ssize_t(*func)(int, const void*, size_t, int);
};

struct sendto_event {
    const void *pc;
    int sockfd;
    const void *buf;
    size_t len;
    int flags;
    const struct sockaddr *dest_addr;
    socklen_t addrlen;
    ssize_t ret;
    ssize_t(*func)(int, const void*, size_t, int, const struct sockaddr*, socklen_t);
};

struct sendmsg_event {
    const void *pc;
    int sockfd;
    const struct msghdr *msg;
    int flags;
    ssize_t ret;
    ssize_t(*func)(int, const struct msghdr*, int);
};

struct shutdown_event {
    const void *pc;
    int sockfd;
    int how;
    int ret;
    int(*func)(int, int);
};

struct socket_event {
    const void *pc;
    int domain;
    int type;
    int protocol;
    int ret;
    int(*func)(int, int, int);
};

struct socketpair_event {
    const void *pc;
    int domain;
    int type;
    int protocol;
    int *sv;
    int ret;
    int(*func)(int, int, int, int*);
};

#endif /* DICE_SOCKET_H */
