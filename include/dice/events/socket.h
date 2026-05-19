/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_SOCKET_H
#define DICE_SOCKET_H

#include <sys/socket.h>

#define EVENT_ACCEPT 104
#define EVENT_ACCEPT4 105

struct accept_event {
    const void *pc;
    int sockfd;
    struct sockaddr* addr;
    socklen_t addrlen;
    int ret;
    int(*func)(int, struct sockaddr*, int sockfd, struct sockaddr*);
};

struct accept4_event {
    const void *pc;
    int sockfd;
    struct sockaddr* addr;
    socklen_t addrlen;
    int flags;
    int ret;
    int(*func)(int, struct sockaddr*, int sockfd, struct sockaddr*, int);
};

#endif /* DICE_SOCKET_H */
