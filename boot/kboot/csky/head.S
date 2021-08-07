/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <size.h>

    .section .setup,"ax"
    
GLOBAL(_start)
    jmpi    kernel_start
END(_start)
     
GLOBAL(kernel_start)
    lrw     r1, CONFIG_PAGE_OFFSET + CONFIG_RAM_BASE
    jmp     r1
END(kernel_start)
  
GLOBAL(halt)
    stop
    jmpi    halt
END(halt)
