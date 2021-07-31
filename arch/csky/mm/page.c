/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */


/*
 *  Kernel page table
 *          ###################################
 *   Addr | 0                2G      2.5G      3G
 *        | #################|++++++++|++++++++|##########
 *  Space |      User        ^ Kernel ^  I/O   ^ Himem
 * 
 *  User:   This part is blank
 *  Kernel: Use fixed huge page mapping
 *  Himem:  Use statically assigned pt_himem
 * 
 */ 
