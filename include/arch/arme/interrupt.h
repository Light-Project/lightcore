/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  include/arch/arme/interrupt.h
 *  arme interrupt define
 *  (C) 2020 wzl finishing
 *
 *  Change Logs:
 *  Date            Notes
 *  2021-02-26      first version
 *
 */

#ifndef _ARCH_ARME_INTERRUPT_H_
#define _ARCH_ARME_INTERRUPT_H_

enum INT_VECTOR{
/*  Mnemonic        Vector     Description                      Source                                                  */
    RST_FAULT       = 1,    /* Reset                            Power-On Reset or Local Reset                           */
    NMI_INT         = 2,    /* NMI                              Non-Maskable Interrupt                                  */
    HF_FAULT        = 3,    /* HardFault                        A fault that no other exception mechanism can handle    */
    MM_FAULT        = 4,    /* MemManage                        MemManage fault                                         */
    BUS_FAULT       = 5,    /* BusFault                         Memory related faults                                   */
    US_FAULT        = 6,    /* UsageFault                       Instruction usage error                                 */
    SVC_INTERRUPT   = 11,   /* MonitorSVCall                    SVC instruction                                         */
    DBG_FAULT       = 12,   /* Debug Monitor                    Watchpoints                                             */
    PSV_INTERRUPT   = 14,   /* Pendsv interrupt                 */
    STK_INTERRUPT   = 15,   /* Systick interrupt                Systick overflow                                        */
};

#endif
