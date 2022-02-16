/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_UCONTEXT_H_
#define _LIGHTCORE_UCONTEXT_H_

#include <types.h>
#include <state.h>
#include <lightcore/signal.h>
#include <asm/kcontext.h>

struct kcontext {
    struct mcontext mcontext;
    struct kcontext *link;
    void *stack;
    unsigned long ssize;
    state retval;
};

/*
 * Get user context and store it in variable pointed to by kc.
 */
extern void getcontext(struct kcontext *kc);

/*
 * Set user context from information of variable pointed to by kc.
 */
extern void __noreturn setcontext(const struct kcontext *kc);

/*
 * Save current context in context variable pointed to by okc and set
 * context from variable pointed to by nkc.
 */
extern struct kcontext *swapcontext(struct kcontext *okc, const struct kcontext *nkc);

/*
 * Manipulate user context kc to continue with calling functions fn
 * and the argc-1 parameters following argc when the context is used
 * the next time in 'setcontext' or 'swapcontext'.
 *
 * We cannot say anything about the parameters FUNC takes; `void'
 * is as good as any other choice.
 */
extern void makecontext(struct kcontext *kc, state (*entry)(void), int argc, ...);

#endif  /* _LIGHTCORE_UCONTEXT_H_ */