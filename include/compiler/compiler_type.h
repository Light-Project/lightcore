#ifndef _COMPILER_TYPE_H_
#define _COMPILER_TYPE_H_
#ifdef __CHECKER__

/* address spaces */
# define __kernel	__attribute__((address_space(0)))
# define __user		__attribute__((noderef, address_space(__user)))
# define __iomem	__attribute__((noderef, address_space(__iomem)))
# define __percpu	__attribute__((noderef, address_space(__percpu)))
# define __rcu		__attribute__((noderef, address_space(__rcu)))
static inline void __chk_user_ptr(const volatile void __user *ptr) { }
static inline void __chk_io_ptr(const volatile void __iomem *ptr) { }
/* context/locking */
# define __must_hold(x)	__attribute__((context(x,1,1)))
# define __acquires(x)	__attribute__((context(x,0,1)))
# define __releases(x)	__attribute__((context(x,1,0)))
# define __acquire(x)	__context__(x,1)
# define __release(x)	__context__(x,-1)
# define __cond_lock(x,c)	((c) ? ({ __acquire(x); 1; }) : 0)
/* other */
# define __force	__attribute__((force))
# define __nocast	__attribute__((nocast))
# define __safe		__attribute__((safe))
# define __private	__attribute__((noderef))
# define ACCESS_PRIVATE(p, member) (*((typeof((p)->member) __force *) &(p)->member))
#else /* __CHECKER__ */
/* address spaces */
# define __kernel
# ifdef STRUCTLEAK_PLUGIN
#  define __user	__attribute__((user))
# else
#  define __user
# endif
# define __iomem
# define __percpu
# define __rcu
# define __chk_user_ptr(x)	(void)0
# define __chk_io_ptr(x)	(void)0
/* context/locking */
# define __must_hold(x)
# define __acquires(x)
# define __releases(x)
# define __acquire(x)	(void)0
# define __release(x)	(void)0
# define __cond_lock(x,c) (c)
/* other */
# define __force
# define __nocast
# define __safe
# define __private
# define ACCESS_PRIVATE(p, member) ((p)->member)
# define __builtin_warning(x, y...) (1)
#endif /* __CHECKER__ */

/* Indirect macros required for expanded argument pasting, eg. __LINE__. */
#define ___PASTE(a,b) a##b
#define __PASTE(a,b) ___PASTE(a,b) 


#endif
