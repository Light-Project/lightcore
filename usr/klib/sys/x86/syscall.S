#include <linkage.h>

/**
 * Arguments:
 * eax  system call number
 * ebx  arg1
 * ecx  arg2
 * edx  arg3
 * esi  arg4
 * edi  arg5
 * ebp  arg6
 */
ENTRY(syscall)
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %ebx

	movl    44(%esp),%ebp   /* Load the 6 syscall argument registers */
	movl    40(%esp),%edi
	movl    36(%esp),%esi
	movl    32(%esp),%edx
	movl    28(%esp),%ecx
	movl    24(%esp),%ebx
	movl    20(%esp),%eax   /* Load syscall number into %eax.  */
    
    int     0x80
    
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
    
    ret
END(syscall)
