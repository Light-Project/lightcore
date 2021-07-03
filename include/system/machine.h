#ifndef __MACHINE_H__
#define __MACHINE_H__

#ifdef __cplusplus
extern "C" {
#endif 

struct machine_t {
    struct kobj_t * kobj;
    struct list_head list;
    struct list_head mmap;

    const char * name;
    const char * desc;

    int (*detect)(struct machine_t * mach);
    void (*memmap)(struct machine_t * mach);
    void (*smpinit)(struct machine_t * mach, int cpu);
    void (*smpboot)(struct machine_t * mach, int cpu, void (*func)(int cpu));
    void (*shutdown)(struct machine_t * mach);
    void (*reboot)(struct machine_t * mach);
    void (*sleep)(struct machine_t * mach);
    void (*cleanup)(struct machine_t * mach);
    void (*logger)(struct machine_t * mach, const char * buf, int count);
    const char * (*uniqueid)(struct machine_t * mach);
    int (*keygen)(struct machine_t * mach, const char * msg, void * key);
};
    
    
#endif /* __MACHINE_H__ */
