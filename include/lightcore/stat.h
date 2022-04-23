/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_STAT_H_
#define _LIGHTCORE_STAT_H_

#define	S_IRWXU     0000700     /* RWX mask for owner */
#define	S_IRUSR     0000400     /* R for owner */
#define	S_IWUSR     0000200     /* W for owner */
#define	S_IXUSR     0000100     /* X for owner */

#define S_IRWXG     0000070     /* RWX mask for group */
#define S_IRGRP     0000040     /* R for group */
#define S_IWGRP     0000020     /* W for group */
#define S_IXGRP     0000010     /* X for group */

#define S_IRWXO	    0000007     /* RWX mask for other */
#define S_IROTH	    0000004     /* R for other */
#define S_IWOTH	    0000002     /* W for other */
#define S_IXOTH	    0000001     /* X for other */

#define S_IFMT      0170000     /* type of file mask */
#define S_IFSOCK    0140000     /* socket */
#define S_IFLNK     0120000     /* symbolic link */
#define S_IFREG     0100000     /* regular */
#define S_IFBLK     0060000     /* block special */
#define S_IFDIR     0040000     /* directory */
#define S_IFCHR     0020000     /* character special */
#define S_IFIFO     0010000     /* named pipe (fifo) */
#define	S_ISUID     0004000     /* set user id on execution */
#define	S_ISGID     0002000     /* set group id on execution */
#define S_ISVTX     0001000     /* save swapped text even after use */

#define S_IRWXUGO   (S_IRWXU | S_IRWXG | S_IRWXO)
#define S_IRUGO     (S_IRUSR | S_IRGRP | S_IRWXO)
#define S_IWUGO     (S_IWUSR | S_IWGRP | S_IWOTH)
#define S_IWUGO     (S_IWUSR | S_IWGRP | S_IWOTH)

#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)
#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK)
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)

#endif  /* _LIGHTCORE_TYPES_H_ */
