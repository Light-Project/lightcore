/**
 * DEFINE_IDTENTRY - Emit code for simple IDT entry points
 * @func:	Function name of the entry point
 *
 * @func is called from ASM entry code with interrupts disabled.
 *
 * The macro is written so it acts as function definition. Append the
 * body with a pair of curly brackets.
 *
 * irqentry_enter() contains common code which has to be invoked before
 * arbitrary code in the body. irqentry_exit() contains common code
 * which has to run before returning to the low level assembly code.
 */
#define DEFINE_IDTENTRY(func)                                   \
static __always_inline void __##func(struct pt_regs *regs);     \
                                                                \
__visible noinstr void func(struct pt_regs *regs)               \
{                                                               \
    irqentry_state_t state = irqentry_enter(regs);              \
                                                                \
	instrumentation_begin();                                    \
	__##func (regs);                                            \
	instrumentation_end();                                      \
	irqentry_exit(regs, state);                                 \
}                                                               \
                                                                \
static __always_inline void __##func(struct pt_regs *regs)