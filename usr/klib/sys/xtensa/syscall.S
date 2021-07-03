#include <linkage.h>

#define SYSCALL_MAX_ARGS 6
#define XTENSA_SYSCALL_ARGUMENT_REGS {6, 3, 4, 5, 8, 9}

/* long syscall(long sysnum, long a, long b, long c, long d, long e, long f) */
ENTRY(syscall)
    addi    sp, sp, -4
    s32i    a6, sp, 0

    mov     a6, a3      /* arg a -> a6 */
    mov     a3, a4      /* arg b -> a3 */
    mov     a4, a5      /* arg c -> a4 */
    s32i    a5, sp, 0   /* arg d -> a5 */
    mov     a9, a8      /* arg f -> a9 */
    mov     a8, a7      /* arg e -> a8 */
    mov     a7, a2      /* sysnum -> a7 */ 
    syscall
    
    addi    sp, sp, 4
    ret
END(syscall)
