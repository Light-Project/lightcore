/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_IDT_H_
#define _ASM_X86_IDT_H_

#include <arch/x86/interrupt.h>
#define TRAP_SYSCALL 0x80

#ifndef __ASSEMBLY__

#include <linkage.h>
#include <entry.h>
#include <hardirq.h>

#define DEFINE_IDTENTRY_NORMAL(handle)                              \
static __always_inline void __trap_##handle(struct regs *regs);     \
                                                                    \
__visible noinline void trap_##handle(struct regs *regs)            \
{                                                                   \
    entry_irqhandle_enter(regs);                                    \
    __trap_##handle(regs);                                          \
    entry_irqhandle_exit(regs);                                     \
}                                                                   \
                                                                    \
static __always_inline void __trap_##handle(struct regs *regs)

#define DEFINE_IDTENTRY_ERRORCODE(handle)                           \
static __always_inline void                                         \
__trap_##handle(struct regs *regs, unsigned long error_code);       \
                                                                    \
__visible noinline void                                             \
trap_##handle(struct regs *regs, unsigned long error_code)          \
{                                                                   \
    entry_irqhandle_enter(regs);                                    \
    __trap_##handle(regs, error_code);                              \
    entry_irqhandle_exit(regs);                                     \
}                                                                   \
                                                                    \
static __always_inline void                                         \
__trap_##handle(struct regs *regs, unsigned long error_code)

#define DEFINE_IDTENTRY_IRQ(handle)                                 \
static __always_inline void                                         \
__trap_##handle(struct regs *regs, unsigned long vector);           \
                                                                    \
__visible noinline void                                             \
trap_##handle(struct regs *regs, unsigned long error_code)          \
{                                                                   \
    entry_irqhandle_enter(regs);                                    \
    hardirq_entry();                                                \
    __trap_##handle(regs, error_code);                              \
    hardirq_exit();                                                 \
    entry_irqhandle_exit(regs);                                     \
}                                                                   \
                                                                    \
static __always_inline void                                         \
__trap_##handle(struct regs *regs, unsigned long vector)

#define DEFINE_IDTENTRY_RAW(handle)                                 \
__visible noinline void trap_##handle(struct regs *regs)

#define DEFINE_IDTENTRY_RAW_ERRORCODE(handle)                       \
__visible noinline void trap_##handle(struct regs *regs, unsigned long error_code)

#define DECLARE_IDTENTRY_NORMAL(vector, handle)                     \
    extern asmlinkage void entry_##handle(void);                    \
    extern __visible void handle(struct regs *regs);

#define DECLARE_IDTENTRY_ERRORCODE(vector, handle)                  \
    extern asmlinkage void entry_##handle(void);                    \
    extern __visible void handle(struct regs *regs, unsigned long error_code);

#define DECLARE_IDTENTRY_IRQ(vector, handle)                        \
    DECLARE_IDTENTRY_ERRORCODE(vector, handle)

#define DECLARE_IDTENTRY_RAW(vector, handle)                        \
    DECLARE_IDTENTRY_NORMAL(vector, handle)

#define DECLARE_IDTENTRY_RAW_ERRORCODE(vector, handle)              \
    DECLARE_IDTENTRY_ERRORCODE(vector, handle)

#ifdef CONFIG_ARCH_X86_64

#define DEFINE_IDTENTRY_IST(handle)                                 \
    DEFINE_IDTENTRY_RAW(handle)

#define DECLARE_IDTENTRY_IST(vector, handle)                        \
    DECLARE_IDTENTRY_RAW(vector, handle);                           \
    __visible void noist_##handle(struct regs *regs);

#define DEFINE_IDTENTRY_DF      DEFINE_IDTENTRY_RAW_ERRORCODE
#define DECLARE_IDTENTRY_DF     DECLARE_IDTENTRY_RAW_ERRORCODE

#else /* !CONFIG_ARCH_X86_64 */

#define DEFINE_IDTENTRY_DF(handle)                                  \
__visible noinline void trap_##handle                               \
(struct regs *regs, unsigned long address, unsigned long error_code)

#define DECLARE_IDTENTRY_DF(vector, handle)                         \
extern asmlinkage void entry_##handle(void);                        \
__visible noinline void trap_##handle                               \
(struct regs *regs, unsigned long address, unsigned long error_code);

#endif /* CONFIG_ARCH_X86_64 */

#ifdef CONFIG_ARCH_X86_64

#define DEFINE_IDTENTRY_MCS         DEFINE_IDTENTRY_IST
#define DECLARE_IDTENTRY_MCS        DECLARE_IDTENTRY_IST
#define DECLARE_IDTENTRY_MCS_USER   DECLARE_IDTENTRY_NOIST

#define DEFINE_IDTENTRY_DEBUG       DEFINE_IDTENTRY_IST
#define DECLARE_IDTENTRY_DEBUG      DECLARE_IDTENTRY_IST
#define DECLARE_IDTENTRY_DEBUG_USER DECLARE_IDTENTRY_NOIST

#endif /* CONFIG_ARCH_X86_64 */

#define DEFINE_IDTENTRY_NMI         DEFINE_IDTENTRY_RAW
#define DECLARE_IDTENTRY_NMI        DECLARE_IDTENTRY_RAW

#else /* __ASSEMBLY__ */

#define DECLARE_IDTENTRY_NORMAL(vector, handle)                     \
    idtentry vector entry_##handle trap_##handle error_code=0

#define DECLARE_IDTENTRY_ERRORCODE(vector, handle)                  \
    idtentry vector entry_##handle trap_##handle error_code=1

#define DECLARE_IDTENTRY_RAW(vector, handle)                        \
    DECLARE_IDTENTRY_NORMAL(vector, handle)

#define DECLARE_IDTENTRY_RAW_ERRORCODE(vector, handle)              \
    DECLARE_IDTENTRY_ERRORCODE(vector, handle)

#define DECLARE_IDTENTRY_IRQ(vector, handle)                        \
    idtentry_irq vector entry_##handle trap_##handle

#define DECLARE_IDTENTRY_NMI(vector, handle)                        \
    idtentry_nmi vector entry_##handle trap_##handle

#define DECLARE_IDTENTRY_DF(vector, handle)                         \
    idtentry_df vector entry_##handle trap_##handle

#ifdef CONFIG_ARCH_X86_64

#define DECLARE_IDTENTRY_MCE(vector, handle)                        \
    idtentry_mce_db vector entry_##handle trap_##handle

#define DECLARE_IDTENTRY_DEBUG(vector, handle)                      \
    idtentry_mce_db vector entry_##handle trap_##handle

#endif /* CONFIG_ARCH_X86_64 */
#endif /* __ASSEMBLY__ */

#ifdef CONFIG_ARCH_X86_32
# define DECLARE_IDTENTRY_DEBUG     DECLARE_IDTENTRY_RAW
# define DEFINE_IDTENTRY_DEBUG      DEFINE_IDTENTRY_RAW
#endif

DECLARE_IDTENTRY_NORMAL(TRAP_DE, divide_error)
DECLARE_IDTENTRY_NORMAL(TRAP_OF, overflow)
DECLARE_IDTENTRY_NORMAL(TRAP_BR, bounds)
DECLARE_IDTENTRY_NORMAL(TRAP_NM, device_not_available)
DECLARE_IDTENTRY_NORMAL(TRAP_SP, spurious_interrupt)
DECLARE_IDTENTRY_NORMAL(TRAP_MF, coprocessor_error)
DECLARE_IDTENTRY_NORMAL(TRAP_XM, simd_error)

DECLARE_IDTENTRY_ERRORCODE(TRAP_TS, invalid_tss)
DECLARE_IDTENTRY_ERRORCODE(TRAP_NP, segment_not_present)
DECLARE_IDTENTRY_ERRORCODE(TRAP_SS, stack_segment)
DECLARE_IDTENTRY_ERRORCODE(TRAP_GP, general_protection)
DECLARE_IDTENTRY_ERRORCODE(TRAP_AC, alignment_check)

DECLARE_IDTENTRY_RAW(TRAP_UD, invalid_opcode)
DECLARE_IDTENTRY_RAW(TRAP_BP, breakpoint)
DECLARE_IDTENTRY_RAW_ERRORCODE(TRAP_PF, page_fault)
DECLARE_IDTENTRY_IRQ(TRAP_DB, generic_interrupt)

DECLARE_IDTENTRY_NMI(TRAP_NMI, nmi_interrupt)
DECLARE_IDTENTRY_DF(TRAP_DF, double_fault)
DECLARE_IDTENTRY_DEBUG(TRAP_DB, debug)

#ifndef __ASSEMBLY__

extern asmlinkage void entry_kthread_return(state (*entry)(void *arg), void *arg);
extern asmlinkage void entry_fork_return(void);
extern asmlinkage void entry_syscall_80(void);
extern asmlinkage void entry_irq_vector(void);

extern void arch_idt_int_gate(irqnr_t nr, void *handle);
extern void arch_idt_setup(void);

#endif /* __ASSEMBLY__ */
#endif /* _ASM_X86_IDT_H_ */
