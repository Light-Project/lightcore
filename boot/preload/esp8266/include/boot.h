#include <lib.h>
#include "romcall.h"

#define OSC_FREQ        (26 * 1000000)
#define CPU_CLK_FREQ    (OSC_FREQ * 4)
#define APB_CLK_FREQ    (OSC_FREQ * 2)
#define UART_CLK_FREQ   (APB_CLK_FREQ)

/* boot/startup.S */
extern void kernel_start(const char *,...);

/* driver/timer.c */
extern void timer_init(void);
extern void timer_stop(void);
extern int time_read(void);
extern void timer_delay(unsigned int ms);

/* driver/uart.c */
extern void uart_init(void);
extern void uart_print(const char *);


