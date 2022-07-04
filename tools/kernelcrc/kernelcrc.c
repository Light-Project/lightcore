/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define be32 uint32_t

#include "../../include/asm-generic/header.h"
#include "../../include/crypto/crc32-table.h"

static void error(const char *str)
{
    fprintf(stderr, "%s\n", str);
    exit(1);
}

int main(int argc, char *argv[])
{
    struct boot_head *boot_head;
    uint32_t old_crc, new_crc;
    struct stat stat;
    int fd;

    if ((fd = open(argv[1], O_RDWR)) < 0)
        error("can not open file");

    if (fstat(fd, &stat) < 0)
        error("fstat error");

    boot_head = mmap(NULL, stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (boot_head == MAP_FAILED)
        error("mmap error");

    if (strcmp("lightcore!", (char *)boot_head->magic)) {
        close(fd);
        remove(argv[1]);
        error("magic error");
    }

    boot_head->size = stat.st_size - sizeof(struct boot_head);
    old_crc = boot_head->crc;
    new_crc = crc32_inline((void *)(boot_head + 1), boot_head->size, ~0);
    if (old_crc != new_crc) {
        boot_head->crc = new_crc;
        printf("kernel crc has been fixed: 0x%x\n", boot_head->crc);
    } else
        printf("the kernel crc already exists");

    boot_head->hsize = sizeof(struct boot_head);
    old_crc = boot_head->hcrc;
    boot_head->hcrc = 0;
    new_crc = crc32_inline((void *)boot_head, sizeof(struct boot_head), ~0);
    if (old_crc != new_crc) {
        boot_head->hcrc = new_crc;
        printf("header crc has been fixed: 0x%x\n", boot_head->hcrc);
    } else {
        boot_head->hcrc = old_crc;
        printf("the header crc already exists");
    }

    close(fd);
    return 0;
}
