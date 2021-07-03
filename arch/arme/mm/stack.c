#include "lc_stdint.h"


/**
 * @defgroup Stack Manager
 *
 *
 * @param Start_addr: Heap memory start address
 * @param Heap_size:  Heap memory size
 * 
 * @return None 
 */
lc_uint32_t* lc_system_thread_stack_setup(lc_uint32_t* p_Stack,
                                   void* entry,
                                   void* exit,
                                   lc_uint32_t param)
{
        
                                                    /*---------ThreadStack----------*/
        *(p_Stack)   = (lc_uint32_t)0x01000000L;    /*  PSR                         */ 
        *(--p_Stack) = (lc_uint32_t)entry;          /*  PC                          */
        *(--p_Stack) = (lc_uint32_t)exit;           /*  LR                          */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  R12                         */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  R3                          */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  R2                          */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  R1                          */
        *(--p_Stack) = (lc_uint32_t)param;          /*  R0                          */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  R11                         */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  R10                         */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  R9                          */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  R8                          */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  R7                          */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  R6                          */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  R5                          */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  R4                          */
#ifdef CONFIG_ARCH_ARM_ARMV7M_FPU
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPSCR                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS0                        */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS1                        */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS2                        */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS3                        */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS4                        */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS5                        */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS6                        */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS7                        */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS8                        */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS9                        */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS10                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS11                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS12                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS13                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS14                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS15                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS16                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS17                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS18                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS19                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS20                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS21                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS22                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS23                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS24                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS25                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS26                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS27                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS28                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS29                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS30                       */
        *(--p_Stack) = (lc_uint32_t)0x00000000L;    /*  FPS31                       */
#endif
                                                    /*-----------EndStack-----------*/
        return p_Stack;
} 
