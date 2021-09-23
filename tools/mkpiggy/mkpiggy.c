/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

static void error(const char *str)
{
    fprintf(stderr, str);
    fprintf(stderr, "\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    struct stat stat;
    int fd;
    
    if (argc != 2) {
        printf("usage: mkpiggy file\n");
        return -1;
    }
    
    if ((fd = open(argv[1], O_RDWR)) < 0)
        error("can not open file");

    if (fstat(fd, &stat) < 0)
        error("fstat error");
    
    printf("/* SPDX-License-Identifier: GPL-2.0-or-later */\n\n");
    printf("    .section .piggydata, \"a\"\n");
    printf("piggy_start:\n");
	printf("    .incbin \"%s\"\n", argv[1]);
    printf("piggy_end:\n");
    
    close(fd);
    return 0;
}
