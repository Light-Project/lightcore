#include <system/syscall.h>

asmlinkage long sys_open(const char *filename, int flags, umode_t mode)
{
    return 0;
}

asmlinkage long sys_close(unsigned int fd)
{
    return 0;
}

asmlinkage long sys_read(unsigned int fd, char *buf, size_t count)
{
    return 0;
}

asmlinkage long sys_write(unsigned int fd, const char *buf, size_t count)
{
    return 0;
}
