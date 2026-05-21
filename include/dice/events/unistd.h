/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_UNISTD_H
#define DICE_UNISTD_H

#include <stdarg.h>
#include <unistd.h>

#define EVENT_ACCESS               137
#define EVENT_ALARM                138
#define EVENT_BRK                  139
#define EVENT_CHDIR                140
#define EVENT_CHROOT               141
#define EVENT_CHOWN                142
#define EVENT_CLOSE                143
#define EVENT_CONFSTR              144
#define EVENT_CRYPT                145
#define EVENT_CTERMID              146
#define EVENT_CUSERID              147
#define EVENT_DUP                  148
#define EVENT_DUP2                 149
#define EVENT_DUP3                 150
#define EVENT_ENCRYPT              151
#define EVENT__EXIT                152
#define EVENT_FACCESSAT            153
#define EVENT_FCHDIR               154
#define EVENT_FCHOWN               155
#define EVENT_FCHOWNAT             156
#define EVENT_GETUID               157
#define EVENT_FDATASYNC            158
#define EVENT_FORK                 159
#define EVENT_FPATHCONF            160
#define EVENT_FSYNC                161
#define EVENT_FTRUNCATE            162
#define EVENT_GETCWD               163
#define EVENT_GETDTABLESIZE        164
#define EVENT_GETEGID              165
#define EVENT_GETEUID              166
#define EVENT_GETGID               167
#define EVENT_GETGROUPS            168
#define EVENT_GETHOSTID            169
#define EVENT_GETLOGIN             170
#define EVENT_GETLOGIN_R           171
#define EVENT_GETOPT               172
#define EVENT_GETPAGESIZE          173
#define EVENT_GETPASS              174
#define EVENT_GETPGID              175
#define EVENT_GETPGRP              176
#define EVENT_GETPID               177
#define EVENT_GETPPID              178
#define EVENT_GETSID               179
#define EVENT_GETWD                180
#define EVENT_GET_CURRENT_DIR_NAME 181
#define EVENT_ISATTY               182
#define EVENT_LCHOWN               183
#define EVENT_LINK                 184
#define EVENT_LINKAT               185
#define EVENT_LOCKF                186
#define EVENT_LSEEK                187
#define EVENT_NICE                 188
#define EVENT_PATHCONF             189
#define EVENT_PAUSE                190
#define EVENT_PIPE                 191
#define EVENT_PIPE2                192
#define EVENT_PREAD                193
#define EVENT_PTHREAD_ATFORK       194
#define EVENT_PWRITE               195
#define EVENT_READ                 196
#define EVENT_READLINK             197
#define EVENT_READLINKAT           198
#define EVENT_RMDIR                199
#define EVENT_SBRK                 200
#define EVENT_SETGID               201
#define EVENT_SETHOSTID            202
#define EVENT_SETPGID              203
#define EVENT_SETPGRP              204
#define EVENT_SETREGID             205
#define EVENT_SETREUID             206
#define EVENT_SETSID               207
#define EVENT_SETUID               208
#define EVENT_SLEEP                209
#define EVENT_SWAB                 210
#define EVENT_SYMLINK              211
#define EVENT_SYMLINKAT            212
#define EVENT_SYNC                 213
#define EVENT_SYNCFS               214
#define EVENT_SYSCONF              215
#define EVENT_TCGETPGRP            216
#define EVENT_TCSETPGRP            217
#define EVENT_TRUNCATE             218
#define EVENT_TTYNAME              219
#define EVENT_TTYNAME_R            220
#define EVENT_UALARM               221
#define EVENT_UNLINK               222
#define EVENT_UNLINKAT             223
#define EVENT_USLEEP               224
#define EVENT_VFORK                225
#define EVENT_WRITE                226

struct access_event {
    const void *pc;
    const char *path;
    int mode;
    int ret;
    int (*func)(const char *, int);
};

struct alarm_event {
    const void *pc;
    unsigned int seconds;
    unsigned int ret;
    unsigned int (*func)(unsigned int);
};

struct brk_event {
    const void *pc;
    void *addr;
    int ret;
    int (*func)(void *);
};

struct chdir_event {
    const void *pc;
    const char *path;
    int ret;
    int (*func)(const char *);
};

struct chroot_event {
    const void *pc;
    const char *path;
    int ret;
    int (*func)(const char *);
};

struct chown_event {
    const void *pc;
    const char *path;
    uid_t owner;
    gid_t group;
    int ret;
    int (*func)(const char *, uid_t, gid_t);
};

struct close_event {
    const void *pc;
    int fd;
    int ret;
    int (*func)(int);
};

struct confstr_event {
    const void *pc;
    int name;
    char *buf;
    size_t size;
    size_t ret;
    size_t (*func)(int, char *, size_t);
};

struct crypt_event {
    const void *pc;
    const char *key;
    const char *salt;
    char *ret;
    char *(*func)(const char *, const char *);
};

struct ctermid_event {
    const void *pc;
    char *s;
    char *ret;
    char *(*func)(char *);
};

struct cuserid_event {
    const void *pc;
    char *string;
    char *ret;
    char *(*func)(char *);
};

struct dup_event {
    const void *pc;
    int oldfd;
    int ret;
    int (*func)(int);
};

struct dup2_event {
    const void *pc;
    int oldfd;
    int newfd;
    int ret;
    int (*func)(int, int);
};

struct dup3_event {
    const void *pc;
    int oldfd;
    int newfd;
    int flags;
    int ret;
    int (*func)(int, int, int);
};

struct encrypt_event {
    const void *pc;
    char block[64];
    int edflag;
    void (*func)(char[64], int);
};

struct _exit_event {
    const void *pc;
    int status;
    void (*func)(int);
};

struct faccessat_event {
    const void *pc;
    int dirfd;
    const char *path;
    int mode;
    int flags;
    int ret;
    int (*func)(int, const char *, int, int);
};

struct fchdir_event {
    const void *pc;
    int fd;
    int ret;
    int (*func)(int);
};

struct fchown_event {
    const void *pc;
    int fildes;
    uid_t owner;
    gid_t group;
    int ret;
    int (*func)(int, uid_t, gid_t);
};

struct fchownat_event {
    const void *pc;
    int fd;
    const char *path;
    uid_t owner;
    gid_t group;
    int flag;
    int ret;
    int (*func)(int, const char *, uid_t, gid_t, int);
};

struct getuid_event {
    const void *pc;
    uid_t ret;
    uid_t (*func)(void);
};

struct fdatasync_event {
    const void *pc;
    int fd;
    int ret;
    int (*func)(int);
};

struct fork_event {
    const void *pc;
    pid_t ret;
    pid_t (*func)(void);
};

struct fpathconf_event {
    const void *pc;
    int fd;
    int name;
    long ret;
    long (*func)(int, int);
};

struct fsync_event {
    const void *pc;
    int fd;
    int ret;
    int (*func)(int);
};

struct ftruncate_event {
    const void *pc;
    int fildes;
    off_t length;
    int ret;
    int (*func)(int, off_t);
};

struct getcwd_event {
    const void *pc;
    size_t size;
    char *buf;
    char *ret;
    char *(*func)(char *, size_t);
};

struct getdtablesize_event {
    const void *pc;
    int ret;
    int (*func)(void);
};

struct getegid_event {
    const void *pc;
    gid_t ret;
    gid_t (*func)(void);
};

struct geteuid_event {
    const void *pc;
    uid_t ret;
    uid_t (*func)(void);
};

struct getgid_event {
    const void *pc;
    gid_t ret;
    gid_t (*func)(void);
};

struct getgroups_event {
    const void *pc;
    int size;
    gid_t *list;
    int ret;
    int (*func)(int, gid_t *);
};

struct gethostid_event {
    const void *pc;
    long ret;
    long (*func)(void);
};

struct getlogin_event {
    const void *pc;
    char *ret;
    char *(*func)(void);
};

struct getlogin_r_event {
    const void *pc;
    char *buf;
    size_t bufsize;
    int ret;
    int (*func)(char *, size_t);
};

struct getopt_event {
    const void *pc;
    int argc;
    char * const *argv;
    const char *optstring;
    int ret;
    int (*func)(int, char * const *, const char *);
};

struct getpagesize_event {
    const void *pc;
    int ret;
    int (*func)(void);
};

struct getpass_event {
    const void *pc;
    const char *prompt;
    char *ret;
    char *(*func)(const char *);
};

struct getpgid_event {
    const void *pc;
    pid_t pid;
    pid_t ret;
    pid_t (*func)(pid_t);
};

struct getpgrp_event {
    const void *pc;
    pid_t ret;
    pid_t (*func)(void);
};

struct getpid_event {
    const void *pc;
    pid_t ret;
    pid_t (*func)(void);
};

struct getppid_event {
    const void *pc;
    pid_t ret;
    pid_t (*func)(void);
};

struct getsid_event {
    const void *pc;
    pid_t pid;
    pid_t ret;
    pid_t (*func)(pid_t);
};

struct getwd_event {
    const void *pc;
    char *buf;
    char *ret;
    char *(*func)(char *);
};

struct get_current_dir_name_event {
    const void *pc;
    char *ret;
    char *(*func)(void);
};

struct isatty_event {
    const void *pc;
    int fd;
    int ret;
    int (*func)(int);
};

struct lchown_event {
    const void *pc;
    const char *path;
    uid_t owner;
    gid_t group;
    int ret;
    int (*func)(const char *, uid_t, gid_t);
};

struct link_event {
    const void *pc;
    const char *oldpath;
    const char *newpath;
    int ret;
    int (*func)(const char *, const char *);
};

struct linkat_event {
    const void *pc;
    int olddirfd;
    const char *oldpath;
    int newdirfd;
    const char *newpath;
    int flags;
    int ret;
    int (*func)(int, const char *, int, const char *, int);
};

struct lockf_event {
    const void *pc;
    int fd;
    int op;
    off_t size;
    int ret;
    int (*func)(int, int, off_t);
};

struct lseek_event {
    const void *pc;
    int fd;
    off_t offset;
    int whence;
    off_t ret;
    off_t (*func)(int, off_t, int);
};

struct nice_event {
    const void *pc;
    int inc;
    int ret;
    int (*func)(int);
};

struct pathconf_event {
    const void *pc;
    const char *path;
    int name;
    long ret;
    long (*func)(const char *, int);
};

struct pause_event {
    const void *pc;
    int ret;
    int (*func)(void);
};

struct pipe_event {
    const void *pc;
    int pipefd[2];
    int ret;
    int (*func)(int[2]);
};

struct pipe2_event {
    const void *pc;
    int pipefd[2];
    int flags;
    int ret;
    int (*func)(int[2], int);
};

struct pread_event {
    const void *pc;
    int fd;
    void *buf;
    size_t count;
    off_t offset;
    ssize_t ret;
    ssize_t (*func)(int, void *, size_t, off_t);
};

struct pthread_atfork_event {
    const void *pc;
    void (*prepare)(void);
    void (*parent)(void);
    void (*child)(void);
    int ret;
    int (*func)(void (*)(void), void (*)(void), void (*)(void));
};

struct pwrite_event {
    const void *pc;
    int fd;
    const void *buf;
    size_t count;
    off_t offset;
    ssize_t ret;
    ssize_t (*func)(int, const void *, size_t, off_t);
};

struct read_event {
    const void *pc;
    int fd;
    void *buf;
    size_t count;
    ssize_t ret;
    ssize_t (*func)(int, void *, size_t);
};

struct readlink_event {
    const void *pc;
    const char *path;
    char *buf;
    size_t bufsiz;
    ssize_t ret;
    ssize_t (*func)(const char *, char *, size_t);
};

struct readlinkat_event {
    const void *pc;
    int dirfd;
    const char *path;
    char *buf;
    size_t bufsiz;
    ssize_t ret;
    ssize_t (*func)(int, const char *, char *, size_t);
};

struct rmdir_event {
    const void *pc;
    const char *path;
    int ret;
    int (*func)(const char *);
};

struct sbrk_event {
    const void *pc;
    intptr_t increment;
    void *ret;
    void *(*func)(intptr_t);
};

struct setgid_event {
    const void *pc;
    gid_t gid;
    int ret;
    int (*func)(gid_t);
};

struct sethostid_event {
    const void *pc;
    long hostid;
    int ret;
    int (*func)(long);
};

struct setpgid_event {
    const void *pc;
    pid_t pid;
    pid_t pgid;
    int ret;
    int (*func)(pid_t, pid_t);
};

struct setpgrp_event {
    const void *pc;
    pid_t ret;
    pid_t (*func)(void);
};

struct setregid_event {
    const void *pc;
    gid_t rgid;
    gid_t egid;
    int ret;
    int (*func)(gid_t, gid_t);
};

struct setreuid_event {
    const void *pc;
    uid_t ruid;
    uid_t euid;
    int ret;
    int (*func)(uid_t, uid_t);
};

struct setsid_event {
    const void *pc;
    pid_t ret;
    pid_t (*func)(void);
};

struct setuid_event {
    const void *pc;
    uid_t uid;
    int ret;
    int (*func)(uid_t);
};

struct sleep_event {
    const void *pc;
    unsigned int seconds;
    unsigned int ret;
    unsigned int (*func)(unsigned int);
};

struct swab_event {
    const void *pc;
    ssize_t n;
    const void *from;
    void *to;
    void (*func)(const void *, void *, ssize_t);
};

struct symlink_event {
    const void *pc;
    const char *target;
    const char *linkpath;
    int ret;
    int (*func)(const char *, const char *);
};

struct symlinkat_event {
    const void *pc;
    const char *target;
    int newdirfd;
    const char *linkpath;
    int ret;
    int (*func)(const char *, int, const char *);
};

struct sync_event {
    const void *pc;
    void (*func)(void);
};

struct syncfs_event {
    const void *pc;
    int fd;
    int ret;
    int (*func)(int);
};

struct sysconf_event {
    const void *pc;
    int name;
    long ret;
    long (*func)(int);
};

struct tcgetpgrp_event {
    const void *pc;
    int fd;
    pid_t ret;
    pid_t (*func)(int);
};

struct tcsetpgrp_event {
    const void *pc;
    int fd;
    pid_t pgrp;
    int ret;
    int (*func)(int, pid_t);
};

struct truncate_event {
    const void *pc;
    const char *path;
    off_t length;
    int ret;
    int (*func)(const char *, off_t);
};

struct ttyname_event {
    const void *pc;
    int fd;
    char *ret;
    char *(*func)(int);
};

struct ttyname_r_event {
    const void *pc;
    int fd;
    char *buf;
    size_t size;
    int ret;
    int (*func)(int, char *, size_t);
};

struct ualarm_event {
    const void *pc;
    useconds_t usecs;
    useconds_t interval;
    useconds_t ret;
    useconds_t (*func)(useconds_t, useconds_t);
};

struct unlink_event {
    const void *pc;
    const char *path;
    int ret;
    int (*func)(const char *);
};

struct unlinkat_event {
    const void *pc;
    int dirfd;
    const char *path;
    int flags;
    int ret;
    int (*func)(int, const char *, int);
};

struct usleep_event {
    const void *pc;
    useconds_t usec;
    int ret;
    int (*func)(useconds_t);
};

struct vfork_event {
    const void *pc;
    pid_t ret;
    pid_t (*func)(void);
};

struct write_event {
    const void *pc;
    int fd;
    const void *buf;
    size_t count;
    ssize_t ret;
    ssize_t (*func)(int, const void *, size_t);
};

#endif /* DICE_UNISTD_H */
