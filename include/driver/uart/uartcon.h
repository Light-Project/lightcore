
#include <driver/uart/uart.h>

struct uartcon_device {
    struct console *con;
    struct uart_port port;
    char options[16];       /* e.g., 115200n8 */
    unsigned int baud;      
};
