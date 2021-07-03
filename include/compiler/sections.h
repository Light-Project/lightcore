#ifndef _SECTIONS_H_
#define _SECTIONS_H_


/* These are for everybody (although not all archs will actually
   discard it in modules) */
#define __init      __section(".init.text") __cold
#define __initdata  __section(".init.data")
#define __initconst __section(".init.rodata")
#define __exitdata  __section(".exit.data")
#define __exit_call __used __section(".exitcall.exit")


#endif
