
struct lc_system_pcb_typedef
{
    unsigned int *sp;
    
    unsigned int pid;
    unsigned char name[20];
    unsigned int* pcb_next;
    enum state{
        thread_running=0,
        thread_wait=1,
        thread_block=2,
        thread_sleep=3
    };
    enum schedule{
        fifo_thread=0,
        rr_thread=1,
        defaults_thread=2
    };
    unsigned int prio;
    unsigned int creat_prio;
    unsigned int creat_time;
    
    unsigned int *entry;
    unsigned int *exit;
    unsigned int stacksize;

};
