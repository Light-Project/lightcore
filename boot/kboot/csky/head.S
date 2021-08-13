/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <size.h>

    .section .startup,"ax"
    
GLOBAL(_start)
    jmpi    halt
END(_start)
  
GLOBAL(halt)
    stop
    jmpi    halt
END(halt)
