#include "include/lc_system.h"



lc_uint8_t lc_thread_lock()
{
    
    
    
    
}

lc_uint8_t lc_thread_unlock()
{
    
    
    
    
}

lc_uint8_t lc_thread_sw()
{
    
    
    lc_thread_hook();
    
}

lc_uint8_t lc_thread_setup(const char* process_name,
                         void (*entry)(*parameter),
                         lc_size_t stack_size,
                         lc_size_t process_prio)
{
    lc_system_pcb_register();
    
}

lc_uint8_t lc_thread_del(const char* process_name)
{
    lc_system_pcb_del(lc_pcb_freepid());
    
}

