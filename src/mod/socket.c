/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/chains/intercept.h>
#include <dice/events/socket.h>
#include <dice/interpose.h>
#include <dice/module.h>
#include <dice/pubsub.h>

INTERPOSE(int, accept, int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    struct accept_event ev = {
        .pc     = INTERPOSE_PC,
        .sockfd = sockfd,
        .addr   = addr,
        .addrlen = addrlen,
        .ret    = 0,
        .func   = REAL_FUNC(accept),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_ACCEPT, &ev, &md);
    ev.ret = ev.func(ev.sockfd, ev.addr, ev.addrlen);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_ACCEPT, &ev, &md);
    return ev.ret;
}

#if !defined(__APPLE__)
INTERPOSE(int, accept4, int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags)
{
    struct accept4_event ev = {
        .pc      = INTERPOSE_PC,
        .sockfd  = sockfd,
        .addr    = addr,
        .addrlen = addrlen,
        .flags   = flags,
        .ret     = 0,
        .func    = REAL_FUNC(accept4),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_ACCEPT4, &ev, &md);
    ev.ret = ev.func(ev.sockfd, ev.addr, ev.addrlen, ev.flags);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_ACCEPT4, &ev, &md);
    return ev.ret;
}
#endif

INTERPOSE(int, bind, int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    struct bind_event ev = {
        .pc     = INTERPOSE_PC,
        .sockfd = sockfd,
        .addr   = addr,
        .addrlen = addrlen,
        .ret    = 0,
        .func   = REAL_FUNC(bind),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_BIND, &ev, &md);
    ev.ret = ev.func(ev.sockfd, ev.addr, ev.addrlen);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_BIND, &ev, &md);

    return ev.ret;
}

INTERPOSE(int, connect, int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    struct connect_event ev = {
        .pc     = INTERPOSE_PC,
        .sockfd = sockfd,
        .addr   = (struct sockaddr *)addr,
        .addrlen = addrlen,
        .ret    = 0,
        .func   = REAL_FUNC(connect),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CONNECT, &ev, &md);
    ev.ret = ev.func(ev.sockfd, ev.addr, ev.addrlen);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CONNECT, &ev, &md);

    return ev.ret;
}

INTERPOSE(int, getpeername, int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    struct getpeername_event ev = {
        .pc      = INTERPOSE_PC,
        .sockfd  = sockfd,
        .addr    = addr,
        .addrlen = addrlen,
        .ret     = 0,
        .func    = REAL_FUNC(getpeername),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETPEERNAME, &ev, &md);
    ev.ret = ev.func(ev.sockfd, ev.addr, ev.addrlen);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETPEERNAME, &ev, &md);

    return ev.ret;
}

INTERPOSE(int, getsockname, int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    struct getsockname_event ev = {
        .pc      = INTERPOSE_PC,
        .sockfd  = sockfd,
        .addr    = addr,
        .addrlen = addrlen,
        .ret     = 0,
        .func    = REAL_FUNC(getsockname),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETSOCKNAME, &ev, &md);
    ev.ret = ev.func(ev.sockfd, ev.addr, ev.addrlen);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETSOCKNAME, &ev, &md);

    return ev.ret;
}

INTERPOSE(int, getsockopt, int sockfd, int level, int optname, void *optval, socklen_t *optlen)
{
    struct getsockopt_event ev = {
        .pc      = INTERPOSE_PC,
        .sockfd  = sockfd,
        .level   = level,
        .optname = optname,
        .optval  = optval,
        .optlen  = optlen,
        .ret     = 0,
        .func    = REAL_FUNC(getsockopt),
    };

    metadata_t md = {0};

    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETSOCKOPT, &ev, &md);
    ev.ret = ev.func(ev.sockfd, ev.level, ev.optname, ev.optval, ev.optlen);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETSOCKOPT, &ev, &md);

    return ev.ret;
}

INTERPOSE(int, setsockopt, int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
    struct setsockopt_event ev = {
        .pc      = INTERPOSE_PC,
        .sockfd  = sockfd,
        .level   = level,
        .optname = optname,
        .optval  = optval,
        .optlen  = optlen,
        .ret     = 0,
        .func    = REAL_FUNC(setsockopt),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SETSOCKOPT, &ev, &md);
    ev.ret = ev.func(ev.sockfd, ev.level, ev.optname, ev.optval, ev.optlen);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SETSOCKOPT, &ev, &md);

    return ev.ret;
}

INTERPOSE(int, listen, int sockfd, int backlog)
{
    struct listen_event ev = {
        .pc     = INTERPOSE_PC,
        .sockfd = sockfd,
        .backlog = backlog,
        .ret    = 0,
        .func   = REAL_FUNC(listen),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_LISTEN, &ev, &md);
    ev.ret = ev.func(ev.sockfd, ev.backlog);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_LISTEN, &ev, &md);

    return ev.ret;
}

INTERPOSE(ssize_t, recv, int sockfd, void *buf, size_t len, int flags)
{
    struct recv_event ev = {
        .pc     = INTERPOSE_PC,
        .sockfd = sockfd,
        .buf    = buf,
        .len    = len,
        .flags  = flags,
        .ret    = 0,
        .func   = REAL_FUNC(recv),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_RECV, &ev, &md);
    ev.ret = ev.func(ev.sockfd, ev.buf, ev.len, ev.flags);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_RECV, &ev, &md);

    return ev.ret;
}

INTERPOSE(ssize_t, recvfrom, int sockfd, void *buf, size_t len, int flags,
           struct sockaddr *src_addr, socklen_t *addrlen)
{
    struct recvfrom_event ev = {
        .pc       = INTERPOSE_PC,
        .sockfd   = sockfd,
        .buf      = buf,
        .len      = len,
        .flags    = flags,
        .src_addr = src_addr,
        .addrlen  = addrlen,
        .ret      = 0,
        .func     = REAL_FUNC(recvfrom),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_RECVFROM, &ev, &md);
    ev.ret = ev.func(ev.sockfd, ev.buf, ev.len, ev.flags, ev.src_addr, ev.addrlen);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_RECVFROM, &ev, &md);

    return ev.ret;
}

INTERPOSE(ssize_t, recvmsg, int sockfd, struct msghdr *msg, int flags)
{
    struct recvmsg_event ev = {
        .pc     = INTERPOSE_PC,
        .sockfd = sockfd,
        .msg    = msg,
        .flags  = flags,
        .ret    = 0,
        .func   = REAL_FUNC(recvmsg),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_RECVMSG, &ev, &md);
    ev.ret = ev.func(ev.sockfd, ev.msg, ev.flags);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_RECVMSG, &ev, &md);

    return ev.ret;
}

INTERPOSE(ssize_t, send, int sockfd, const void *buf, size_t len, int flags)
{
    struct send_event ev = {
        .pc     = INTERPOSE_PC,
        .sockfd = sockfd,
        .buf    = buf,
        .len    = len,
        .flags  = flags,
        .ret    = 0,
        .func   = REAL_FUNC(send),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SEND, &ev, &md);
    ev.ret = ev.func(ev.sockfd, ev.buf, ev.len, ev.flags);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SEND, &ev, &md);

    return ev.ret;
}

INTERPOSE(ssize_t, sendto, int sockfd, const void *buf, size_t len, int flags,
           const struct sockaddr *dest_addr, socklen_t addrlen)
{
    struct sendto_event ev = {
        .pc        = INTERPOSE_PC,
        .sockfd    = sockfd,
        .buf       = buf,
        .len       = len,
        .flags     = flags,
        .dest_addr = dest_addr,
        .addrlen   = addrlen,
        .ret       = 0,
        .func      = REAL_FUNC(sendto),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SENDTO, &ev, &md);
    ev.ret = ev.func(ev.sockfd, ev.buf, ev.len, ev.flags, ev.dest_addr, ev.addrlen);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SENDTO, &ev, &md);

    return ev.ret;
}

INTERPOSE(ssize_t, sendmsg, int sockfd, const struct msghdr *msg, int flags)
{
    struct sendmsg_event ev = {
        .pc     = INTERPOSE_PC,
        .sockfd = sockfd,
        .msg    = msg,
        .flags  = flags,
        .ret    = 0,
        .func   = REAL_FUNC(sendmsg),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SENDMSG, &ev, &md);
    ev.ret = ev.func(ev.sockfd, ev.msg, ev.flags);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SENDMSG, &ev, &md);

    return ev.ret;
}

INTERPOSE(int, shutdown, int sockfd, int how)
{
    struct shutdown_event ev = {
        .pc     = INTERPOSE_PC,
        .sockfd = sockfd,
        .how    = how,
        .ret    = 0,
        .func   = REAL_FUNC(shutdown),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SHUTDOWN, &ev, &md);
    ev.ret = ev.func(ev.sockfd, ev.how);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SHUTDOWN, &ev, &md);

    return ev.ret;
}

INTERPOSE(int, socket, int domain, int type, int protocol)
{
    struct socket_event ev = {
        .pc       = INTERPOSE_PC,
        .domain   = domain,
        .type     = type,
        .protocol = protocol,
        .ret      = 0,
        .func     = REAL_FUNC(socket),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SOCKET, &ev, &md);
    ev.ret = ev.func(ev.domain, ev.type, ev.protocol);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SOCKET, &ev, &md);

    return ev.ret;
}

#if defined(__NetBSD__)
INTERPOSE(int, socketpair, int domain, int type, int protocol, int *sv)
#else
INTERPOSE(int, socketpair, int domain, int type, int protocol, int sv[2])
#endif
{
    struct socketpair_event ev = {
        .pc       = INTERPOSE_PC,
        .domain   = domain,
        .type     = type,
        .protocol = protocol,
        .sv       = sv,
        .func     = REAL_FUNC(socketpair),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SOCKETPAIR, &ev, &md);
    ev.ret = ev.func(ev.domain, ev.type, ev.protocol, ev.sv);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SOCKETPAIR, &ev, &md);

    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_ACCEPT)
PS_ADVERTISE_TYPE(EVENT_ACCEPT4)
PS_ADVERTISE_TYPE(EVENT_BIND)
PS_ADVERTISE_TYPE(EVENT_CONNECT)
PS_ADVERTISE_TYPE(EVENT_GETPEERNAME)
PS_ADVERTISE_TYPE(EVENT_GETSOCKNAME)
PS_ADVERTISE_TYPE(EVENT_SETSOCKOPT)
PS_ADVERTISE_TYPE(EVENT_LISTEN)
PS_ADVERTISE_TYPE(EVENT_RECV)
PS_ADVERTISE_TYPE(EVENT_RECVFROM)
PS_ADVERTISE_TYPE(EVENT_RECVMSG)
PS_ADVERTISE_TYPE(EVENT_SEND)
PS_ADVERTISE_TYPE(EVENT_SENDTO)
PS_ADVERTISE_TYPE(EVENT_SENDMSG)
PS_ADVERTISE_TYPE(EVENT_SHUTDOWN)
PS_ADVERTISE_TYPE(EVENT_SOCKET)
PS_ADVERTISE_TYPE(EVENT_SOCKETPAIR)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
