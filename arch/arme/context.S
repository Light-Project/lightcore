/*
 *  SPDX-License-Identifier: GPLV2
 *  lightcore/arch/arm/v7m/context.c
 *  Armv7m serial context manager
 *  (C) 2020 wzl
 */
    
    .cpu    cortex-m3
    .fpu    softvfp
    .syntax unified
    .thumb
    .text
    
    .equ NVIC_INT_CTRL,0xE000ED04 
    .equ NVIC_PENDSVSET,0x10000000
    
.globl lc_system_thread_sw

lc_system_thread_sw:
    PUSH {R0,R1}
    
    LDR R0,=NVIC_INT_CTRL
    LDR R1,=NVIC_PENDSVSET
    STR R1,[R0]
    
    POP {R0,R1}
    
    BX LR

PendSV_Handler:
    PUSH  {LR}
    
    bl  lc_thread_sw
    
    POP {PC}
    
context_save:
    
    PUSH  {R0,LR}
    MRS   R0,PSP
    SUBS  R0,#0x20
    STM   R0, {R4-R11}
    
    LDR   R1,=ThreadSp
    STR   R0,[R1]
    
    BL    SAVE_ThreadEnv 
    POP	  {R0,LR}
    BX    LR
    
context_sw:
    PUSH {R0,R1}
    
    LDR	R1,=ThreadSp
    LDR R0,[R1]
    
    LDM R0,{R4-R11}
    ADD R0,#0x20
    MSR PSP,R0

    POP  {R0,R1}

    BX  LR
    
    .global PendSV_Handler
    .type PendSV_Handler, %function


    
