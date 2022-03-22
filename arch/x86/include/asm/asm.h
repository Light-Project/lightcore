/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_ASM_H_
#define _ASM_X86_ASM_H_

#ifdef __ASSEMBLY__
# define __ASM_FORM(x)          x
# define __ASM_FORM_RAW(x)      x
# define __ASM_FORM_COMMA(x)    x,
#else
# define __ASM_FORM(x)          " " __stringify(x) " "
# define __ASM_FORM_RAW(x)      __stringify(x)
# define __ASM_FORM_COMMA(x)    " " __stringify(x) ","
#endif

#ifdef CONFIG_ARCH_X86_32
# define __ASM_SEL(a, b)        __ASM_FORM(a)
# define __ASM_SEL_RAW(a, b)    __ASM_FORM_RAW(a)
#else
# define __ASM_SEL(a, b)        __ASM_FORM(b)
# define __ASM_SEL_RAW(a, b)    __ASM_FORM_RAW(b)
#endif

#define __ASM_SIZE(inst, ...)   __ASM_SEL(inst##l##__VA_ARGS__, inst##q##__VA_ARGS__)
#define __ASM_REG(reg)          __ASM_SEL_RAW(e##reg, r##reg)

#define _ASM_PTR                __ASM_SEL(.long, .quad)
#define _ASM_ALIGN              __ASM_SEL(.balign 4, .balign 8)

#define _ASM_MOV                __ASM_SIZE(mov)
#define _ASM_INC                __ASM_SIZE(inc)
#define _ASM_DEC                __ASM_SIZE(dec)
#define _ASM_ADD                __ASM_SIZE(add)
#define _ASM_SUB                __ASM_SIZE(sub)
#define _ASM_XADD               __ASM_SIZE(xadd)
#define _ASM_MUL                __ASM_SIZE(mul)

#define _ASM_AX                 __ASM_REG(ax)
#define _ASM_BX                 __ASM_REG(bx)
#define _ASM_CX                 __ASM_REG(cx)
#define _ASM_DX                 __ASM_REG(dx)
#define _ASM_SP                 __ASM_REG(sp)
#define _ASM_BP                 __ASM_REG(bp)
#define _ASM_SI                 __ASM_REG(si)
#define _ASM_DI                 __ASM_REG(di)

#ifdef __GCC_ASM_FLAG_OUTPUTS__
# define CC_SET(c) "\n\t/* output condition code " #c "*/\n"
# define CC_OUT(c) "=@cc" #c
#else
# define CC_SET(c) "\n\tset" #c " %[_cc_" #c "]\n"
# define CC_OUT(c) [_cc_ ## c] "=qm"
#endif

#ifdef CONFIG_ARCH_X86_64
# define DECLARE_ARGS(val, low, high)   unsigned long low, high
# define EAX_EDX_VAL(val, low, high)    ((low) | (high) << 32)
# define EAX_EDX_RET(val, low, high)    "=a" (low), "=d" (high)
#else
# define DECLARE_ARGS(val, low, high)   unsigned long long val
# define EAX_EDX_VAL(val, low, high)    (val)
# define EAX_EDX_RET(val, low, high)    "=A" (val)
#endif

#endif /* _ASM_X86_ASM_H_ */
