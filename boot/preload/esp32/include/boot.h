#include <lib.h>

#define MHZ             (1000000)
#define OSC_FREQ        (40 * MHZ)
#define CPU_CLK_FREQ    (OSC_FREQ * 6)
#define APB_CLK_FREQ    (OSC_FREQ * 2)
#define REF_CLK_FREQ    (MHZ)
#define UART_CLK_FREQ   (REF_CLK_FREQ)

/* boot/startup.S */
extern void kernel_start(const char *,...);


/* driver/uart.c */
extern void uart_init(void);
extern void uart_print(const char *);





