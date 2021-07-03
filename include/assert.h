#ifndef _ASSERT_H_ 
#define _ASSERT_H_

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef CONFIG_ASSERT
#define assert(x) ((void)0)
#else
#define assert(x) ((void)((x) || assert_down(__FILE__,__LINE__,__func__,#x)))
#endif
    
#ifdef __cplusplus
}
#endif

#endif
