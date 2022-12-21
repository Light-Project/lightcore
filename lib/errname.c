/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <errname.h>
#include <kernel.h>
#include <bsearch.h>
#include <export.h>

#define ERRNAME(errnum, infos) {errnum, #errnum, infos}
const struct errname_entry errname_table[] = {
    ERRNAME(ENOERR, "No error"),
    ERRNAME(EPERM, "Operation not permitted"),
    ERRNAME(ENOENT, "No such file or directory"),
    ERRNAME(ESRCH, "No such process"),
    ERRNAME(EINTR, "Interrupted system call"),
    ERRNAME(EIO, "I/O error"),
    ERRNAME(ENXIO, "No such device or address"),
    ERRNAME(E2BIG, "Argument list too long"),
    ERRNAME(ENOEXEC, "Exec format error"),
    ERRNAME(EBADF, "Bad file number"),
    ERRNAME(ECHILD, "No child processes"),
    ERRNAME(EAGAIN, "Try again"),
    ERRNAME(ENOMEM, "Out of memory"),
    ERRNAME(EACCES, "Permission denied"),
    ERRNAME(EFAULT, "Bad address"),
    ERRNAME(ENOTBLK, "Block device required"),
    ERRNAME(EBUSY, "Device or resource busy"),
    ERRNAME(EEXIST, "File exists"),
    ERRNAME(EXDEV, "Cross-device link"),
    ERRNAME(ENODEV, "No such device"),
    ERRNAME(ENOTDIR, "Not a directory"),
    ERRNAME(EISDIR, "Is a directory"),
    ERRNAME(EINVAL, "Invalid argument"),
    ERRNAME(ENFILE, "File table overflow"),
    ERRNAME(EMFILE, "Too many open files"),
    ERRNAME(ENOTTY, "Not a typewriter"),
    ERRNAME(ETXTBSY, "Text file busy"),
    ERRNAME(EFBIG, "File too large"),
    ERRNAME(ENOSPC, "No space left on device"),
    ERRNAME(ESPIPE, "Illegal seek"),
    ERRNAME(EROFS, "Read-only file system"),
    ERRNAME(EMLINK, "Too many links"),
    ERRNAME(EPIPE, "Broken pipe"),
    ERRNAME(EDOM, "Math argument out of domain of func"),
    ERRNAME(ERANGE, "Math result not representable"),

    ERRNAME(EDEADLK, "Resource deadlock would occur"),
    ERRNAME(ENAMETOOLONG, "File name too long"),
    ERRNAME(ENOLCK, "No record locks available"),
    ERRNAME(ENOSYS, "Invalid system call number"),
    ERRNAME(EBFONT, "Bad font file format"),
    ERRNAME(ENOSTR, "Device not a stream"),
    ERRNAME(ENODATA, "No data available"),
    ERRNAME(ETIME, "Timer expired"),
    ERRNAME(ENOSR, "Out of streams resources"),
    ERRNAME(ENONET, "Machine is not on the network"),
    ERRNAME(ENOPKG, "Package not installed"),
    ERRNAME(EREMOTE, "Object is remote"),
    ERRNAME(ENOLINK, "Link has been severed"),
    ERRNAME(EADV, "Advertise error"),
    ERRNAME(ESRMNT, "Srmount error"),
    ERRNAME(ECOMM, "Communication error on send"),
    ERRNAME(EPROTO, "Protocol error"),
    ERRNAME(EMULTIHOP, "Multihop attempted"),
    ERRNAME(EDOTDOT, "RFS specific error"),
    ERRNAME(EBADMSG, "Not a data message"),
    ERRNAME(EOVERFLOW, "Value too large for defined data type"),
    ERRNAME(ENOTUNIQ, "Name not unique on network"),
    ERRNAME(EBADFD, "File descriptor in bad state"),
    ERRNAME(EREMCHG, "Remote address changed"),
    ERRNAME(ELIBACC, "Can not access a needed shared library"),
    ERRNAME(ELIBBAD, "Accessing a corrupted shared library"),
    ERRNAME(ELIBSCN, ".lib section in a.out corrupted"),
    ERRNAME(ELIBMAX, "Attempting to link in too many shared libraries"),
    ERRNAME(ELIBEXEC, "Cannot exec a shared library directly"),
    ERRNAME(EILSEQ, "Illegal byte sequence"),
    ERRNAME(ERESTART, "Interrupted system call should be restarted"),
    ERRNAME(ESTRPIPE, "Streams pipe error"),
    ERRNAME(EUSERS, "Too many users"),
    ERRNAME(ENOTSOCK, "Socket operation on non-socket"),
    ERRNAME(EDESTADDRREQ, "Destination address required"),
    ERRNAME(EMSGSIZE, "Message too long"),
    ERRNAME(EPROTOTYPE, "Protocol wrong type for socket"),
    ERRNAME(ENOPROTOOPT, "Protocol not available"),
    ERRNAME(EPROTONOSUPPORT, "Protocol not supported"),
    ERRNAME(ESOCKTNOSUPPORT, "Socket type not supported"),
    ERRNAME(EOPNOTSUPP, "Operation not supported on transport endpoint"),
    ERRNAME(EPFNOSUPPORT, "Protocol family not supported"),
    ERRNAME(EAFNOSUPPORT, "Address family not supported by protocol"),
    ERRNAME(EADDRINUSE, "Address already in use"),
    ERRNAME(EADDRNOTAVAIL, "Cannot assign requested address"),
    ERRNAME(ENETDOWN, "Network is down"),
    ERRNAME(ENETUNREACH, "Network is unreachable"),
    ERRNAME(ENETRESET, "Network dropped connection because of reset"),
    ERRNAME(ECONNABORTED, "Software caused connection abort"),
    ERRNAME(ECONNRESET, "Connection reset by peer"),
    ERRNAME(ENOBUFS, "No buffer space available"),
    ERRNAME(EISCONN, "Transport endpoint is already connected"),
    ERRNAME(ENOTCONN, "Transport endpoint is not connected"),
    ERRNAME(ESHUTDOWN, "Cannot send after transport endpoint shutdown"),
    ERRNAME(ETOOMANYREFS, "Too many references: cannot splice"),
    ERRNAME(ETIMEDOUT, "Connection timed out"),
    ERRNAME(ECONNREFUSED, "Connection refused"),
    ERRNAME(EHOSTDOWN, "Host is down"),
    ERRNAME(EHOSTUNREACH, "No route to host"),
    ERRNAME(EALREADY, "Operation already in progress"),
    ERRNAME(EINPROGRESS, "Operation now in progress"),
    ERRNAME(ESTALE, "Stale file handle"),
    ERRNAME(EUCLEAN, "Structure needs cleaning"),
    ERRNAME(ENOTNAM, "Not a XENIX named type file"),
    ERRNAME(ENAVAIL, "No XENIX semaphores available"),
    ERRNAME(EISNAM, "Is a named type file"),
    ERRNAME(EREMOTEIO, "Remote I/O error"),
    ERRNAME(EDQUOT, "Quota exceeded"),
    ERRNAME(ENOMEDIUM, "No medium found"),
    ERRNAME(EMEDIUMTYPE, "Wrong medium type"),
    ERRNAME(ECANCELED, "Operation Canceled"),
    ERRNAME(ENOKEY, "Required key not available"),
    ERRNAME(EKEYEXPIRED, "Key has expired"),
    ERRNAME(EKEYREVOKED, "Key has been revoked"),
    ERRNAME(EKEYREJECTED, "Key was rejected by service"),

    ERRNAME(ERESTARTSYS, "restart if system call.."),
    ERRNAME(ERESTARTNOINTR, "restart if no interrupt.."),
    ERRNAME(ERESTARTNOHAND, "restart if no handler.."),
    ERRNAME(ENOIOCTLCMD, "No ioctl command"),
    ERRNAME(ERESTART_RESTARTBLOCK, "restart by calling sys_restart_syscall"),
    ERRNAME(EPROBE_DEFER, "Driver requests probe retry"),
    ERRNAME(EOPENSTALE, "open found a stale dentry"),
    ERRNAME(ENOPARAM, "Parameter not supported"),

    ERRNAME(EBADHANDLE, "Illegal NFS file handle"),
    ERRNAME(ENOTSYNC, "Update synchronization mismatch"),
    ERRNAME(EBADCOOKIE, "Cookie is stale"),
    ERRNAME(ENOTSUPP, "Operation is not supported"),
    ERRNAME(ETOOSMALL, "Buffer or request is too small"),
    ERRNAME(ESERVERFAULT, "An untranslatable error occurred"),
    ERRNAME(EBADTYPE, "Type not supported by server"),
    ERRNAME(EJUKEBOX, "Request initiated, but will not complete before timeout"),
    ERRNAME(EIOCBQUEUED, "iocb queued, will get completion event"),
    ERRNAME(ERECALLCONFLICT, "conflict with recalled state"),
    ERRNAME(ENOGRACE, "NFS file lock reclaim refused"),

    { }, /* NULL */
};
EXPORT_SYMBOL(errname_table);

static long errname_search(const void *key, void *pdata)
{
    const struct errname_entry *entry = key;
    return entry->errnum - (state)pdata;
}

struct errname_entry *errname_find(state error)
{
    struct errname_entry *entry;

    if (unlikely(error > 0))
        return NULL;

    entry = bsearch(
        errname_table, ARRAY_SIZE(errname_table) - 1,
        sizeof(*entry), errname_search, (void *)-error
    );

    return entry;
}
EXPORT_SYMBOL(errname_find);

const char *errname(state error, const char **infop)
{
    struct errname_entry *entry;

    entry = errname_find(error);
    if (entry && infop)
        *infop = entry->info;

    return entry ? entry->name : NULL;
}
EXPORT_SYMBOL(errname);
