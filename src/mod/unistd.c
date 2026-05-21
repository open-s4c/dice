/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/chains/intercept.h>
#include <dice/events/unistd.h>
#include <dice/interpose.h>
#include <dice/module.h>
#include <dice/pubsub.h>

INTERPOSE(int, access, const char *path, int mode)
{
    struct access_event ev = {
        .pc   = INTERPOSE_PC,
        .path = path,
        .mode = mode,
        .ret  = 0,
        .func = REAL_FUNC(access),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_ACCESS, &ev, &md);
    ev.ret = ev.func(ev.path, ev.mode);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_ACCESS, &ev, &md);
    return ev.ret;
}

INTERPOSE(unsigned int, alarm, unsigned int seconds)
{
    struct alarm_event ev = {
        .pc      = INTERPOSE_PC,
        .seconds = seconds,
        .ret     = 0,
        .func    = REAL_FUNC(alarm),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_ALARM, &ev, &md);
    ev.ret = ev.func(ev.seconds);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_ALARM, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, brk, void *addr)
{
    struct brk_event ev = {
        .pc   = INTERPOSE_PC,
        .addr = addr,
        .ret  = 0,
        .func = REAL_FUNC(brk),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_BRK, &ev, &md);
    ev.ret = ev.func(ev.addr);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_BRK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, chdir, const char *path)
{
    struct chdir_event ev = {
        .pc   = INTERPOSE_PC,
        .path = path,
        .ret  = 0,
        .func = REAL_FUNC(chdir),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CHDIR, &ev, &md);
    ev.ret = ev.func(ev.path);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CHDIR, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, chroot, const char *path)
{
    struct chroot_event ev = {
        .pc   = INTERPOSE_PC,
        .path = path,
        .ret  = 0,
        .func = REAL_FUNC(chroot),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CHROOT, &ev, &md);
    ev.ret = ev.func(ev.path);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CHROOT, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, chown, const char *path, uid_t owner, gid_t group)
{
    struct chown_event ev = {
        .pc    = INTERPOSE_PC,
        .path  = path,
        .owner = owner,
        .group = group,
        .ret   = 0,
        .func  = REAL_FUNC(chown),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CHOWN, &ev, &md);
    ev.ret = ev.func(ev.path, ev.owner, ev.group);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CHOWN, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, close, int fd)
{
    struct close_event ev = {
        .pc   = INTERPOSE_PC,
        .fd   = fd,
        .ret  = 0,
        .func = REAL_FUNC(close),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CLOSE, &ev, &md);
    ev.ret = ev.func(ev.fd);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CLOSE, &ev, &md);
    return ev.ret;
}

INTERPOSE(size_t, confstr, int name, char *buf, size_t size)
{
    struct confstr_event ev = {
        .pc   = INTERPOSE_PC,
        .name = name,
        .buf  = buf,
        .size = size,
        .ret  = 0,
        .func = REAL_FUNC(confstr),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CONFSTR, &ev, &md);
    ev.ret = ev.func(ev.name, ev.buf, ev.size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CONFSTR, &ev, &md);
    return ev.ret;
}

INTERPOSE(char *, crypt, const char *key, const char *salt)
{
    struct crypt_event ev = {
        .pc   = INTERPOSE_PC,
        .key  = key,
        .salt = salt,
        .ret  = NULL,
        .func = REAL_FUNC(crypt),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CRYPT, &ev, &md);
    ev.ret = ev.func(ev.key, ev.salt);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CRYPT, &ev, &md);
    return ev.ret;
}

INTERPOSE(char *, ctermid, char *s)
{
    struct ctermid_event ev = {
        .pc  = INTERPOSE_PC,
        .s   = s,
        .ret = NULL,
        .func = REAL_FUNC(ctermid),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CTERMID, &ev, &md);
    ev.ret = ev.func(ev.s);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CTERMID, &ev, &md);
    return ev.ret;
}

INTERPOSE(char *, cuserid, char *string)
{
    struct cuserid_event ev = {
        .pc   = INTERPOSE_PC,
        .string = string,
        .ret  = NULL,
        .func = REAL_FUNC(cuserid),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CUSERID, &ev, &md);
    ev.ret = ev.func(ev.string);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CUSERID, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, dup, int oldfd)
{
    struct dup_event ev = {
        .pc    = INTERPOSE_PC,
        .oldfd = oldfd,
        .ret   = 0,
        .func  = REAL_FUNC(dup),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_DUP, &ev, &md);
    ev.ret = ev.func(ev.oldfd);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_DUP, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, dup2, int oldfd, int newfd)
{
    struct dup2_event ev = {
        .pc    = INTERPOSE_PC,
        .oldfd = oldfd,
        .newfd = newfd,
        .ret   = 0,
        .func  = REAL_FUNC(dup2),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_DUP2, &ev, &md);
    ev.ret = ev.func(ev.oldfd, ev.newfd);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_DUP2, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, dup3, int oldfd, int newfd, int flags)
{
    struct dup3_event ev = {
        .pc    = INTERPOSE_PC,
        .oldfd = oldfd,
        .newfd = newfd,
        .flags = flags,
        .ret   = 0,
        .func  = REAL_FUNC(dup3),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_DUP3, &ev, &md);
    ev.ret = ev.func(ev.oldfd, ev.newfd, ev.flags);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_DUP3, &ev, &md);
    return ev.ret;
}

#if defined(__NetBSD__)
INTERPOSE(void, encrypt, char *block, int edflag)
#else
INTERPOSE(void, encrypt, char block[64], int edflag)
#endif
{
    struct encrypt_event ev = {
        .pc     = INTERPOSE_PC,
        .edflag = edflag,
        .func   = REAL_FUNC(encrypt),
    };
    /* Copy the block to the struct */
    for (int i = 0; i < 64; i++) ev.block[i] = block[i];

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_ENCRYPT, &ev, &md);
    ev.func(ev.block, ev.edflag);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_ENCRYPT, &ev, &md);

    /* Copy back result to original block */
    for (int i = 0; i < 64; i++) block[i] = ev.block[i];
}

INTERPOSE(void, _exit, int status)
{
    struct _exit_event ev = {
        .pc     = INTERPOSE_PC,
        .status = status,
        .func   = REAL_FUNC(_exit),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT__EXIT, &ev, &md);
    ev.func(ev.status);
    __builtin_unreachable();
}

INTERPOSE(int, faccessat, int dirfd, const char *path, int mode, int flags)
{
    struct faccessat_event ev = {
        .pc    = INTERPOSE_PC,
        .dirfd = dirfd,
        .path  = path,
        .mode  = mode,
        .flags = flags,
        .ret   = 0,
        .func  = REAL_FUNC(faccessat),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_FACCESSAT, &ev, &md);
    ev.ret = ev.func(ev.dirfd, ev.path, ev.mode, ev.flags);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_FACCESSAT, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, fchdir, int fd)
{
    struct fchdir_event ev = {
        .pc   = INTERPOSE_PC,
        .fd   = fd,
        .ret  = 0,
        .func = REAL_FUNC(fchdir),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_FCHDIR, &ev, &md);
    ev.ret = ev.func(ev.fd);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_FCHDIR, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, fchown, int fildes, uid_t owner, gid_t group)
{
    struct fchown_event ev = {
        .pc     = INTERPOSE_PC,
        .fildes = fildes,
        .owner  = owner,
        .group  = group,
        .ret    = 0,
        .func   = REAL_FUNC(fchown),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_FCHOWN, &ev, &md);
    ev.ret = ev.func(ev.fildes, ev.owner, ev.group);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_FCHOWN, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, fchownat, int fd, const char *path, uid_t owner, gid_t group, int flag)
{
    struct fchownat_event ev = {
        .pc    = INTERPOSE_PC,
        .fd    = fd,
        .path  = path,
        .owner = owner,
        .group = group,
        .flag  = flag,
        .ret   = 0,
        .func  = REAL_FUNC(fchownat),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_FCHOWNAT, &ev, &md);
    ev.ret = ev.func(ev.fd, ev.path, ev.owner, ev.group, ev.flag);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_FCHOWNAT, &ev, &md);
    return ev.ret;
}

INTERPOSE(uid_t, getuid, void)
{
    struct getuid_event ev = {
        .pc   = INTERPOSE_PC,
        .ret  = 0,
        .func = REAL_FUNC(getuid),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETUID, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETUID, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, fdatasync, int fd)
{
    struct fdatasync_event ev = {
        .pc   = INTERPOSE_PC,
        .fd   = fd,
        .ret  = 0,
        .func = REAL_FUNC(fdatasync),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_FDATASYNC, &ev, &md);
    ev.ret = ev.func(ev.fd);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_FDATASYNC, &ev, &md);
    return ev.ret;
}

INTERPOSE(pid_t, fork, void)
{
    struct fork_event ev = {
        .pc   = INTERPOSE_PC,
        .ret  = 0,
        .func = REAL_FUNC(fork),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_FORK, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_FORK, &ev, &md);
    return ev.ret;
}

INTERPOSE(long, fpathconf, int fd, int name)
{
    struct fpathconf_event ev = {
        .pc   = INTERPOSE_PC,
        .fd   = fd,
        .name = name,
        .ret  = 0,
        .func = REAL_FUNC(fpathconf),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_FPATHCONF, &ev, &md);
    ev.ret = ev.func(ev.fd, ev.name);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_FPATHCONF, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, fsync, int fd)
{
    struct fsync_event ev = {
        .pc   = INTERPOSE_PC,
        .fd   = fd,
        .ret  = 0,
        .func = REAL_FUNC(fsync),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_FSYNC, &ev, &md);
    ev.ret = ev.func(ev.fd);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_FSYNC, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, ftruncate, int fildes, off_t length)
{
    struct ftruncate_event ev = {
        .pc     = INTERPOSE_PC,
        .fildes = fildes,
        .length = length,
        .ret    = 0,
        .func   = REAL_FUNC(ftruncate),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_FTRUNCATE, &ev, &md);
    ev.ret = ev.func(ev.fildes, ev.length);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_FTRUNCATE, &ev, &md);
    return ev.ret;
}

INTERPOSE(char *, getcwd, char *buf, size_t size)
{
    struct getcwd_event ev = {
        .pc   = INTERPOSE_PC,
        .buf  = buf,
        .size = size,
        .ret  = NULL,
        .func = REAL_FUNC(getcwd),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETCWD, &ev, &md);
    ev.ret = ev.func(ev.buf, ev.size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETCWD, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, getdtablesize, void)
{
    struct getdtablesize_event ev = {
        .pc   = INTERPOSE_PC,
        .ret  = 0,
        .func = REAL_FUNC(getdtablesize),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETDTABLESIZE, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETDTABLESIZE, &ev, &md);
    return ev.ret;
}

INTERPOSE(gid_t, getegid, void)
{
    struct getegid_event ev = {
        .pc   = INTERPOSE_PC,
        .ret  = 0,
        .func = REAL_FUNC(getegid),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETEGID, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETEGID, &ev, &md);
    return ev.ret;
}

INTERPOSE(uid_t, geteuid, void)
{
    struct geteuid_event ev = {
        .pc   = INTERPOSE_PC,
        .ret  = 0,
        .func = REAL_FUNC(geteuid),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETEUID, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETEUID, &ev, &md);
    return ev.ret;
}

INTERPOSE(gid_t, getgid, void)
{
    struct getgid_event ev = {
        .pc   = INTERPOSE_PC,
        .ret  = 0,
        .func = REAL_FUNC(getgid),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETGID, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETGID, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, getgroups, int size, gid_t list[])
{
    struct getgroups_event ev = {
        .pc   = INTERPOSE_PC,
        .size = size,
        .list = list,
        .ret  = 0,
        .func = REAL_FUNC(getgroups),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETGROUPS, &ev, &md);
    ev.ret = ev.func(ev.size, ev.list);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETGROUPS, &ev, &md);
    return ev.ret;
}

INTERPOSE(long, gethostid, void)
{
    struct gethostid_event ev = {
        .pc   = INTERPOSE_PC,
        .ret  = 0,
        .func = REAL_FUNC(gethostid),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETHOSTID, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETHOSTID, &ev, &md);
    return ev.ret;
}

INTERPOSE(char *, getlogin, void)
{
    struct getlogin_event ev = {
        .pc   = INTERPOSE_PC,
        .ret  = NULL,
        .func = REAL_FUNC(getlogin),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETLOGIN, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETLOGIN, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, getlogin_r, char *buf, size_t bufsize)
{
    struct getlogin_r_event ev = {
        .pc      = INTERPOSE_PC,
        .buf     = buf,
        .bufsize = bufsize,
        .ret     = 0,
        .func    = REAL_FUNC(getlogin_r),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETLOGIN_R, &ev, &md);
    ev.ret = ev.func(ev.buf, ev.bufsize);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETLOGIN_R, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, getopt, int argc, char * const *argv, const char *optstring)
{
    struct getopt_event ev = {
        .pc        = INTERPOSE_PC,
        .argc      = argc,
        .argv      = argv,
        .optstring = optstring,
        .ret       = 0,
        .func      = REAL_FUNC(getopt),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETOPT, &ev, &md);
    ev.ret = ev.func(ev.argc, ev.argv, ev.optstring);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETOPT, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, getpagesize, void)
{
    struct getpagesize_event ev = {
        .pc   = INTERPOSE_PC,
        .ret  = 0,
        .func = REAL_FUNC(getpagesize),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETPAGESIZE, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETPAGESIZE, &ev, &md);
    return ev.ret;
}

INTERPOSE(char *, getpass, const char *prompt)
{
    struct getpass_event ev = {
        .pc     = INTERPOSE_PC,
        .prompt = prompt,
        .ret    = NULL,
        .func   = REAL_FUNC(getpass),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETPASS, &ev, &md);
    ev.ret = ev.func(ev.prompt);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETPASS, &ev, &md);
    return ev.ret;
}

INTERPOSE(pid_t, getpgid, pid_t pid)
{
    struct getpgid_event ev = {
        .pc  = INTERPOSE_PC,
        .pid = pid,
        .ret = 0,
        .func = REAL_FUNC(getpgid),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETPGID, &ev, &md);
    ev.ret = ev.func(ev.pid);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETPGID, &ev, &md);
    return ev.ret;
}

INTERPOSE(pid_t, getpgrp, void)
{
    struct getpgrp_event ev = {
        .pc  = INTERPOSE_PC,
        .ret = 0,
        .func = REAL_FUNC(getpgrp),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETPGRP, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETPGRP, &ev, &md);
    return ev.ret;
}

INTERPOSE(pid_t, getpid, void)
{
    struct getpid_event ev = {
        .pc  = INTERPOSE_PC,
        .ret = 0,
        .func = REAL_FUNC(getpid),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETPID, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETPID, &ev, &md);
    return ev.ret;
}

INTERPOSE(pid_t, getppid, void)
{
    struct getppid_event ev = {
        .pc  = INTERPOSE_PC,
        .ret = 0,
        .func = REAL_FUNC(getppid),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETPPID, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETPPID, &ev, &md);
    return ev.ret;
}

INTERPOSE(pid_t, getsid, pid_t pid)
{
    struct getsid_event ev = {
        .pc  = INTERPOSE_PC,
        .pid = pid,
        .ret = 0,
        .func = REAL_FUNC(getsid),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETSID, &ev, &md);
    ev.ret = ev.func(ev.pid);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETSID, &ev, &md);
    return ev.ret;
}

INTERPOSE(char *, getwd, char *buf)
{
    struct getwd_event ev = {
        .pc  = INTERPOSE_PC,
        .buf = buf,
        .ret = NULL,
        .func = REAL_FUNC(getwd),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETWD, &ev, &md);
    ev.ret = ev.func(ev.buf);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETWD, &ev, &md);
    return ev.ret;
}

INTERPOSE(char *, get_current_dir_name, void)
{
    struct get_current_dir_name_event ev = {
        .pc  = INTERPOSE_PC,
        .ret = NULL,
        .func = REAL_FUNC(get_current_dir_name),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GET_CURRENT_DIR_NAME, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GET_CURRENT_DIR_NAME, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, isatty, int fd)
{
    struct isatty_event ev = {
        .pc  = INTERPOSE_PC,
        .fd  = fd,
        .ret = 0,
        .func = REAL_FUNC(isatty),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_ISATTY, &ev, &md);
    ev.ret = ev.func(ev.fd);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_ISATTY, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, lchown, const char *path, uid_t owner, gid_t group)
{
    struct lchown_event ev = {
        .pc    = INTERPOSE_PC,
        .path  = path,
        .owner = owner,
        .group = group,
        .ret   = 0,
        .func  = REAL_FUNC(lchown),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_LCHOWN, &ev, &md);
    ev.ret = ev.func(ev.path, ev.owner, ev.group);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_LCHOWN, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, link, const char *oldpath, const char *newpath)
{
    struct link_event ev = {
        .pc      = INTERPOSE_PC,
        .oldpath = oldpath,
        .newpath = newpath,
        .ret     = 0,
        .func    = REAL_FUNC(link),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_LINK, &ev, &md);
    ev.ret = ev.func(ev.oldpath, ev.newpath);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_LINK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, linkat, int olddirfd, const char *oldpath,
         int newdirfd, const char *newpath, int flags)
{
    struct linkat_event ev = {
        .pc        = INTERPOSE_PC,
        .olddirfd  = olddirfd,
        .oldpath   = oldpath,
        .newdirfd  = newdirfd,
        .newpath   = newpath,
        .flags     = flags,
        .ret       = 0,
        .func      = REAL_FUNC(linkat),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_LINKAT, &ev, &md);
    ev.ret = ev.func(ev.olddirfd, ev.oldpath, ev.newdirfd, ev.newpath, ev.flags);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_LINKAT, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, lockf, int fd, int op, off_t size)
{
    struct lockf_event ev = {
        .pc   = INTERPOSE_PC,
        .fd   = fd,
        .op   = op,
        .size = size,
        .ret  = 0,
        .func = REAL_FUNC(lockf),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_LOCKF, &ev, &md);
    ev.ret = ev.func(ev.fd, ev.op, ev.size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_LOCKF, &ev, &md);
    return ev.ret;
}

INTERPOSE(off_t, lseek, int fd, off_t offset, int whence)
{
    struct lseek_event ev = {
        .pc     = INTERPOSE_PC,
        .fd     = fd,
        .offset = offset,
        .whence = whence,
        .ret    = 0,
        .func   = REAL_FUNC(lseek),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_LSEEK, &ev, &md);
    ev.ret = ev.func(ev.fd, ev.offset, ev.whence);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_LSEEK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, nice, int inc)
{
    struct nice_event ev = {
        .pc  = INTERPOSE_PC,
        .inc = inc,
        .ret = 0,
        .func = REAL_FUNC(nice),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_NICE, &ev, &md);
    ev.ret = ev.func(ev.inc);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_NICE, &ev, &md);
    return ev.ret;
}

INTERPOSE(long, pathconf, const char *path, int name)
{
    struct pathconf_event ev = {
        .pc   = INTERPOSE_PC,
        .path = path,
        .name = name,
        .ret  = 0,
        .func = REAL_FUNC(pathconf),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_PATHCONF, &ev, &md);
    ev.ret = ev.func(ev.path, ev.name);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_PATHCONF, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pause, void)
{
    struct pause_event ev = {
        .pc  = INTERPOSE_PC,
        .ret = 0,
        .func = REAL_FUNC(pause),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_PAUSE, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_PAUSE, &ev, &md);
    return ev.ret;
}

#if defined(__NetBSD__)
INTERPOSE(int, pipe, int *pipefd)
#else
INTERPOSE(int, pipe, int pipefd[2])
#endif
{
    struct pipe_event ev = {
        .pc = INTERPOSE_PC,
        .ret = 0,
        .func = REAL_FUNC(pipe)
    };
    ev.pipefd[0] = pipefd[0];
    ev.pipefd[1] = pipefd[1];

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_PIPE, &ev, &md);
    ev.ret = ev.func(pipefd);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_PIPE, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pipe2, int pipefd[2], int flags)
{
    struct pipe2_event ev = {
        .pc = INTERPOSE_PC,
        .flags = flags,
        .ret = 0,
        .func = REAL_FUNC(pipe2)
    };
    ev.pipefd[0] = pipefd[0];
    ev.pipefd[1] = pipefd[1];

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_PIPE2, &ev, &md);
    ev.ret = ev.func(pipefd, flags);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_PIPE2, &ev, &md);
    return ev.ret;
}

INTERPOSE(ssize_t, pread, int fd, void *buf, size_t count, off_t offset)
{
    struct pread_event ev = {
        .pc = INTERPOSE_PC,
        .fd = fd,
        .buf = buf,
        .count = count,
        .offset = offset,
        .ret = 0,
        .func = REAL_FUNC(pread)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_PREAD, &ev, &md);
    ev.ret = ev.func(ev.fd, ev.buf, ev.count, ev.offset);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_PREAD, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_atfork, void (*prepare)(void), void (*parent)(void), void (*child)(void))
{
    struct pthread_atfork_event ev = {
        .pc = INTERPOSE_PC,
        .prepare = prepare,
        .parent = parent,
        .child = child,
        .ret = 0,
        .func = REAL_FUNC(pthread_atfork)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_PTHREAD_ATFORK, &ev, &md);
    ev.ret = ev.func(ev.prepare, ev.parent, ev.child);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_PTHREAD_ATFORK, &ev, &md);
    return ev.ret;
}

INTERPOSE(ssize_t, pwrite, int fd, const void *buf, size_t count, off_t offset)
{
    struct pwrite_event ev = {
        .pc = INTERPOSE_PC,
        .fd = fd,
        .buf = buf,
        .count = count,
        .offset = offset,
        .ret = 0,
        .func = REAL_FUNC(pwrite)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_PWRITE, &ev, &md);
    ev.ret = ev.func(ev.fd, ev.buf, ev.count, ev.offset);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_PWRITE, &ev, &md);
    return ev.ret;
}

INTERPOSE(ssize_t, read, int fd, void *buf, size_t count)
{
    struct read_event ev = {
        .pc = INTERPOSE_PC,
        .fd = fd,
        .buf = buf,
        .count = count,
        .ret = 0,
        .func = REAL_FUNC(read)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_READ, &ev, &md);
    ev.ret = ev.func(ev.fd, ev.buf, ev.count);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_READ, &ev, &md);
    return ev.ret;
}

INTERPOSE(ssize_t, readlink, const char *path, char *buf, size_t bufsiz)
{
    struct readlink_event ev = {
        .pc = INTERPOSE_PC,
        .path = path,
        .buf = buf,
        .bufsiz = bufsiz,
        .ret = 0,
        .func = REAL_FUNC(readlink)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_READLINK, &ev, &md);
    ev.ret = ev.func(ev.path, ev.buf, ev.bufsiz);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_READLINK, &ev, &md);
    return ev.ret;
}

INTERPOSE(ssize_t, readlinkat, int dirfd, const char *path, char *buf, size_t bufsiz)
{
    struct readlinkat_event ev = {
        .pc = INTERPOSE_PC,
        .dirfd = dirfd,
        .path = path,
        .buf = buf,
        .bufsiz = bufsiz,
        .ret = 0,
        .func = REAL_FUNC(readlinkat)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_READLINKAT, &ev, &md);
    ev.ret = ev.func(ev.dirfd, ev.path, ev.buf, ev.bufsiz);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_READLINKAT, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, rmdir, const char *path)
{
    struct rmdir_event ev = {
        .pc = INTERPOSE_PC,
        .path = path,
        .ret = 0,
        .func = REAL_FUNC(rmdir)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_RMDIR, &ev, &md);
    ev.ret = ev.func(ev.path);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_RMDIR, &ev, &md);
    return ev.ret;
}

INTERPOSE(void *, sbrk, intptr_t increment)
{
    struct sbrk_event ev = {
        .pc = INTERPOSE_PC,
        .increment = increment,
        .ret = NULL,
        .func = REAL_FUNC(sbrk)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SBRK, &ev, &md);
    ev.ret = ev.func(ev.increment);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SBRK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, setgid, gid_t gid)
{
    struct setgid_event ev = {
        .pc = INTERPOSE_PC,
        .gid = gid,
        .ret = 0,
        .func = REAL_FUNC(setgid)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SETGID, &ev, &md);
    ev.ret = ev.func(ev.gid);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SETGID, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, sethostid, long hostid)
{
    struct sethostid_event ev = {
        .pc = INTERPOSE_PC,
        .hostid = hostid,
        .ret = 0,
        .func = REAL_FUNC(sethostid)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SETHOSTID, &ev, &md);
    ev.ret = ev.func(ev.hostid);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SETHOSTID, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, setpgid, pid_t pid, pid_t pgid)
{
    struct setpgid_event ev = {
        .pc = INTERPOSE_PC,
        .pid = pid,
        .pgid = pgid,
        .ret = 0,
        .func = REAL_FUNC(setpgid)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SETPGID, &ev, &md);
    ev.ret = ev.func(ev.pid, ev.pgid);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SETPGID, &ev, &md);
    return ev.ret;
}

#if !defined(__NetBSD__)
INTERPOSE(pid_t, setpgrp, void)
{
    struct setpgrp_event ev = {
        .pc = INTERPOSE_PC,
        .ret = 0,
        .func = REAL_FUNC(setpgrp)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SETPGRP, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SETPGRP, &ev, &md);
    return ev.ret;
}
#endif

INTERPOSE(int, setregid, gid_t rgid, gid_t egid)
{
    struct setregid_event ev = {
        .pc = INTERPOSE_PC,
        .rgid = rgid,
        .egid = egid,
        .ret = 0,
        .func = REAL_FUNC(setregid)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SETREGID, &ev, &md);
    ev.ret = ev.func(ev.rgid, ev.egid);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SETREGID, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, setreuid, uid_t ruid, uid_t euid)
{
    struct setreuid_event ev = {
        .pc = INTERPOSE_PC,
        .ruid = ruid,
        .euid = euid,
        .ret = 0,
        .func = REAL_FUNC(setreuid)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SETREUID, &ev, &md);
    ev.ret = ev.func(ev.ruid, ev.euid);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SETREUID, &ev, &md);
    return ev.ret;
}

INTERPOSE(pid_t, setsid, void)
{
    struct setsid_event ev = {
        .pc = INTERPOSE_PC,
        .ret = 0,
        .func = REAL_FUNC(setsid)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SETSID, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SETSID, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, setuid, uid_t uid)
{
    struct setuid_event ev = {
        .pc = INTERPOSE_PC,
        .uid = uid,
        .ret = 0,
        .func = REAL_FUNC(setuid)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SETUID, &ev, &md);
    ev.ret = ev.func(ev.uid);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SETUID, &ev, &md);
    return ev.ret;
}

INTERPOSE(unsigned int, sleep, unsigned int seconds)
{
    struct sleep_event ev = {
        .pc = INTERPOSE_PC,
        .seconds = seconds,
        .ret = 0,
        .func = REAL_FUNC(sleep)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SLEEP, &ev, &md);
    ev.ret = ev.func(ev.seconds);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SLEEP, &ev, &md);
    return ev.ret;
}

INTERPOSE(void, swab, const void *from, void *to, ssize_t n)
{
    struct swab_event ev = {
        .pc = INTERPOSE_PC,
        .from = from,
        .to = to,
        .n = n,
        .func = REAL_FUNC(swab)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SWAB, &ev, &md);
    ev.func(ev.from, ev.to, ev.n);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SWAB, &ev, &md);
}

INTERPOSE(int, symlink, const char *target, const char *linkpath)
{
    struct symlink_event ev = {
        .pc = INTERPOSE_PC,
        .target = target,
        .linkpath = linkpath,
        .ret = 0,
        .func = REAL_FUNC(symlink)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SYMLINK, &ev, &md);
    ev.ret = ev.func(ev.target, ev.linkpath);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SYMLINK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, symlinkat, const char *target, int newdirfd, const char *linkpath)
{
    struct symlinkat_event ev = {
        .pc = INTERPOSE_PC,
        .target = target,
        .newdirfd = newdirfd,
        .linkpath = linkpath,
        .ret = 0,
        .func = REAL_FUNC(symlinkat)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SYMLINKAT, &ev, &md);
    ev.ret = ev.func(ev.target, ev.newdirfd, ev.linkpath);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SYMLINKAT, &ev, &md);
    return ev.ret;
}

INTERPOSE(void, sync, void)
{
    struct sync_event ev = {
        .pc = INTERPOSE_PC,
        .func = REAL_FUNC(sync)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SYNC, &ev, &md);
    ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SYNC, &ev, &md);
}

INTERPOSE(int, syncfs, int fd)
{
    struct syncfs_event ev = {
        .pc = INTERPOSE_PC,
        .fd = fd,
        .ret = 0,
        .func = REAL_FUNC(syncfs)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SYNCFS, &ev, &md);
    ev.ret = ev.func(ev.fd);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SYNCFS, &ev, &md);
    return ev.ret;
}

INTERPOSE(long, sysconf, int name)
{
    struct sysconf_event ev = {
        .pc = INTERPOSE_PC,
        .name = name,
        .ret = 0,
        .func = REAL_FUNC(sysconf)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SYSCONF, &ev, &md);
    ev.ret = ev.func(ev.name);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SYSCONF, &ev, &md);
    return ev.ret;
}

INTERPOSE(pid_t, tcgetpgrp, int fd)
{
    struct tcgetpgrp_event ev = {
        .pc = INTERPOSE_PC,
        .fd = fd,
        .ret = 0,
        .func = REAL_FUNC(tcgetpgrp)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_TCGETPGRP, &ev, &md);
    ev.ret = ev.func(ev.fd);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_TCGETPGRP, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, tcsetpgrp, int fd, pid_t pgrp)
{
    struct tcsetpgrp_event ev = {
        .pc = INTERPOSE_PC,
        .fd = fd,
        .pgrp = pgrp,
        .ret = 0,
        .func = REAL_FUNC(tcsetpgrp)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_TCSETPGRP, &ev, &md);
    ev.ret = ev.func(ev.fd, ev.pgrp);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_TCSETPGRP, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, truncate, const char *path, off_t length)
{
    struct truncate_event ev = {
        .pc = INTERPOSE_PC,
        .path = path,
        .length = length,
        .ret = 0,
        .func = REAL_FUNC(truncate)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_TRUNCATE, &ev, &md);
    ev.ret = ev.func(ev.path, ev.length);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_TRUNCATE, &ev, &md);
    return ev.ret;
}

INTERPOSE(char *, ttyname, int fd)
{
    struct ttyname_event ev = {
        .pc = INTERPOSE_PC,
        .fd = fd,
        .ret = NULL,
        .func = REAL_FUNC(ttyname)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_TTYNAME, &ev, &md);
    ev.ret = ev.func(ev.fd);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_TTYNAME, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, ttyname_r, int fd, char *buf, size_t size)
{
    struct ttyname_r_event ev = {
        .pc = INTERPOSE_PC,
        .fd = fd,
        .buf = buf,
        .size = size,
        .ret = 0,
        .func = REAL_FUNC(ttyname_r)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_TTYNAME_R, &ev, &md);
    ev.ret = ev.func(ev.fd, ev.buf, ev.size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_TTYNAME_R, &ev, &md);
    return ev.ret;
}

INTERPOSE(useconds_t, ualarm, useconds_t usecs, useconds_t interval)
{
    struct ualarm_event ev = {
        .pc = INTERPOSE_PC,
        .usecs = usecs,
        .interval = interval,
        .ret = 0,
        .func = REAL_FUNC(ualarm)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_UALARM, &ev, &md);
    ev.ret = ev.func(ev.usecs, ev.interval);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_UALARM, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, unlink, const char *path)
{
    struct unlink_event ev = {
        .pc = INTERPOSE_PC,
        .path = path,
        .ret = 0,
        .func = REAL_FUNC(unlink)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_UNLINK, &ev, &md);
    ev.ret = ev.func(ev.path);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_UNLINK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, unlinkat, int dirfd, const char *path, int flags)
{
    struct unlinkat_event ev = {
        .pc = INTERPOSE_PC,
        .dirfd = dirfd,
        .path = path,
        .flags = flags,
        .ret = 0,
        .func = REAL_FUNC(unlinkat)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_UNLINKAT, &ev, &md);
    ev.ret = ev.func(ev.dirfd, ev.path, ev.flags);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_UNLINKAT, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, usleep, useconds_t usec)
{
    struct usleep_event ev = {
        .pc = INTERPOSE_PC,
        .usec = usec,
        .ret = 0,
        .func = REAL_FUNC(usleep)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_USLEEP, &ev, &md);
    ev.ret = ev.func(ev.usec);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_USLEEP, &ev, &md);
    return ev.ret;
}

INTERPOSE(pid_t, vfork, void)
{
    struct vfork_event ev = {
        .pc = INTERPOSE_PC,
        .ret = 0,
        .func = REAL_FUNC(vfork)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_VFORK, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_VFORK, &ev, &md);
    return ev.ret;
}

INTERPOSE(ssize_t, write, int fd, const void *buf, size_t count)
{
    struct write_event ev = {
        .pc = INTERPOSE_PC,
        .fd = fd,
        .buf = buf,
        .count = count,
        .ret = 0,
        .func = REAL_FUNC(write)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_WRITE, &ev, &md);
    ev.ret = ev.func(ev.fd, ev.buf, ev.count);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_WRITE, &ev, &md);
    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_ACCESS)
PS_ADVERTISE_TYPE(EVENT_ALARM)
PS_ADVERTISE_TYPE(EVENT_BRK)
PS_ADVERTISE_TYPE(EVENT_CHDIR)
PS_ADVERTISE_TYPE(EVENT_CHROOT)
PS_ADVERTISE_TYPE(EVENT_CHOWN)
PS_ADVERTISE_TYPE(EVENT_CLOSE)
PS_ADVERTISE_TYPE(EVENT_CONFSTR)
PS_ADVERTISE_TYPE(EVENT_CRYPT)
PS_ADVERTISE_TYPE(EVENT_CTERMID)
PS_ADVERTISE_TYPE(EVENT_CUSERID)
PS_ADVERTISE_TYPE(EVENT_DUP)
PS_ADVERTISE_TYPE(EVENT_DUP2)
PS_ADVERTISE_TYPE(EVENT_DUP3)
PS_ADVERTISE_TYPE(EVENT_ENCRYPT)
PS_ADVERTISE_TYPE(EVENT__EXIT)
PS_ADVERTISE_TYPE(EVENT_FACCESSAT)
PS_ADVERTISE_TYPE(EVENT_FCHDIR)
PS_ADVERTISE_TYPE(EVENT_FCHOWN)
PS_ADVERTISE_TYPE(EVENT_FCHOWNAT)
PS_ADVERTISE_TYPE(EVENT_GETUID)
PS_ADVERTISE_TYPE(EVENT_FDATASYNC)
PS_ADVERTISE_TYPE(EVENT_FORK)
PS_ADVERTISE_TYPE(EVENT_FPATHCONF)
PS_ADVERTISE_TYPE(EVENT_FSYNC)
PS_ADVERTISE_TYPE(EVENT_FTRUNCATE)
PS_ADVERTISE_TYPE(EVENT_GETCWD)
PS_ADVERTISE_TYPE(EVENT_GETDTABLESIZE)
PS_ADVERTISE_TYPE(EVENT_GETEGID)
PS_ADVERTISE_TYPE(EVENT_GETEUID)
PS_ADVERTISE_TYPE(EVENT_GETGID)
PS_ADVERTISE_TYPE(EVENT_GETGROUPS)
PS_ADVERTISE_TYPE(EVENT_GETHOSTID)
PS_ADVERTISE_TYPE(EVENT_GETLOGIN)
PS_ADVERTISE_TYPE(EVENT_GETLOGIN_R)
PS_ADVERTISE_TYPE(EVENT_GETOPT)
PS_ADVERTISE_TYPE(EVENT_GETPAGESIZE)
PS_ADVERTISE_TYPE(EVENT_GETPASS)
PS_ADVERTISE_TYPE(EVENT_GETPGID)
PS_ADVERTISE_TYPE(EVENT_GETPGRP)
PS_ADVERTISE_TYPE(EVENT_GETPID)
PS_ADVERTISE_TYPE(EVENT_GETPPID)
PS_ADVERTISE_TYPE(EVENT_GETSID)
PS_ADVERTISE_TYPE(EVENT_GETWD)
PS_ADVERTISE_TYPE(EVENT_GET_CURRENT_DIR_NAME)
PS_ADVERTISE_TYPE(EVENT_ISATTY)
PS_ADVERTISE_TYPE(EVENT_LCHOWN)
PS_ADVERTISE_TYPE(EVENT_LINK)
PS_ADVERTISE_TYPE(EVENT_LINKAT)
PS_ADVERTISE_TYPE(EVENT_LOCKF)
PS_ADVERTISE_TYPE(EVENT_LSEEK)
PS_ADVERTISE_TYPE(EVENT_NICE)
PS_ADVERTISE_TYPE(EVENT_PATHCONF)
PS_ADVERTISE_TYPE(EVENT_PAUSE)
PS_ADVERTISE_TYPE(EVENT_PIPE)
PS_ADVERTISE_TYPE(EVENT_PIPE2)
PS_ADVERTISE_TYPE(EVENT_PREAD)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_ATFORK)
PS_ADVERTISE_TYPE(EVENT_PWRITE)
PS_ADVERTISE_TYPE(EVENT_READ)
PS_ADVERTISE_TYPE(EVENT_READLINK)
PS_ADVERTISE_TYPE(EVENT_READLINKAT)
PS_ADVERTISE_TYPE(EVENT_RMDIR)
PS_ADVERTISE_TYPE(EVENT_SBRK)
PS_ADVERTISE_TYPE(EVENT_SETGID)
PS_ADVERTISE_TYPE(EVENT_SETHOSTID)
PS_ADVERTISE_TYPE(EVENT_SETPGID)
PS_ADVERTISE_TYPE(EVENT_SETPGRP)
PS_ADVERTISE_TYPE(EVENT_SETREGID)
PS_ADVERTISE_TYPE(EVENT_SETREUID)
PS_ADVERTISE_TYPE(EVENT_SETSID)
PS_ADVERTISE_TYPE(EVENT_SETUID)
PS_ADVERTISE_TYPE(EVENT_SLEEP)
PS_ADVERTISE_TYPE(EVENT_SWAB)
PS_ADVERTISE_TYPE(EVENT_SYMLINK)
PS_ADVERTISE_TYPE(EVENT_SYMLINKAT)
PS_ADVERTISE_TYPE(EVENT_SYNC)
PS_ADVERTISE_TYPE(EVENT_SYNCFS)
PS_ADVERTISE_TYPE(EVENT_SYSCONF)
PS_ADVERTISE_TYPE(EVENT_TCGETPGRP)
PS_ADVERTISE_TYPE(EVENT_TCSETPGRP)
PS_ADVERTISE_TYPE(EVENT_TRUNCATE)
PS_ADVERTISE_TYPE(EVENT_TTYNAME)
PS_ADVERTISE_TYPE(EVENT_TTYNAME_R)
PS_ADVERTISE_TYPE(EVENT_UALARM)
PS_ADVERTISE_TYPE(EVENT_UNLINK)
PS_ADVERTISE_TYPE(EVENT_UNLINKAT)
PS_ADVERTISE_TYPE(EVENT_USLEEP)
PS_ADVERTISE_TYPE(EVENT_VFORK)
PS_ADVERTISE_TYPE(EVENT_WRITE)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
