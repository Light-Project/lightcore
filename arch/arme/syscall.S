/*
 *  SPDX-License-Identifier: GPLV2
 *  lightcore/arch/arm/v7m/syscall.s
 *  Armv7m serial syscall system
 *  (C) 2020 wzl
 */
#include "system/syscall/syscall_tbl.h"
    
    .cpu    cortex-m3
    .fpu    softvfp
    .syntax unified
    .thumb
    .text

.globl restart
lc_restart:
    svc callnum_restart
    bx  lr
    
.globl lc_exit
lc_exit:
    svc callnum_exit
    bx  lr
    
.globl lc_fork
lc_fork:
    svc 2
    bx  lr
    
.globl lc_read
lc_read:
    svc 3
    bx  lr
    
.globl lc_write
lc_write:
    svc 4
    bx  lr
    
.globl lc_open
lc_open:
    svc 5
    bx  lr
    
.globl lc_close
lc_close:
    svc 6
    bx  lr
    
.globl lc_waitpid
lc_waitpid:
    svc 7
    bx  lr
    
.globl lc_cread
lc_cread:
    svc 8
    bx  lr
    
.globl lc_link
lc_link:
    svc 9
    bx  lr
    
.globl lc_connect
lc_connect:
    svc 10
    bx  lr
    
.globl lc_disconnect
lc_disconnect:
    svc 11
    bx  lr
    
.globl lc_exe
lc_exe:
    svc 12
    bx  lr
    
.globl lc_kill
lc_kill:
    svc 13
    bx  lr
    

SVC_Handler:
//         TST     LR,#0x04    //lr bit2=0: use msp , lr bit4=1: use psp
//         ITE     EQ
//         MRSEQ   R0,MSP      //load msp to protect
//         MRSNE   R0,PSP      //load psp to protect
//         
//         PUSH    {R1}        //protect
//         LDR     R1,[R0,#24]
//         LDRB    R1,[R1,#-2]
//         
//         LDR     R0,[R0]
//         CBNZ    R1,SYSCALL_0
        
// SYSCALL_0:
//         CMP     R1,#0
//         BNE     SYSCALL_1
//         POP		{R1}
//         B		Kernel_Mode_Server	
//         
// SYSCALL_1:
//         CMP     R1,#1
//         BNE     SYSCALL_2
//         POP		{R1}
//         B		Kernel_Mode_Server	
// SYSCALL_2:	
//         CMP		R1,#2
//         BNE		SYSCALL_3
//         POP		{R1}
//         B		Kill_Auto_Service
// SYSCALL_3:
//         CMP		R1,#3
//         BNE		SYSCALL_4
//         POP		{R1}
//         B		Kill_Thread_Service
// SYSCALL_4:
//         CMP		R1,#4
//         BNE		SYSCALL_5
//         POP		{R1}
//         B		Enter_Critial_Server
// SYSCALL_5:
//         CMP		R1,#5
//         BNE		SYSCALL_6
//         POP		{R1}
//         B		Exit_Critial_Server
// SYSCALL_6:
//         CMP		R1,#6
//         BNE		SYSCALL_7
//         POP		{R1}
//         B		Syscall_Server6
// SYSCALL_7:
//         CMP		R1,#7
//         BNE		SYSCALL_8
//         POP		{R1}
//         B		Syscall_Server7
// SYSCALL_8:
//         CMP		R1,#8
//         BNE		SYSCALL_ERROR
//         POP		{R1}
//         B		Syscall_Server8
//     
// SYSCALL_ERROR:
//         POP 	{LR}
            
