#include <printk.h>
#include <export.h>

int snprintf(char * buf, size_t n, const char * fmt, ...)
{
	va_list ap;
	int rv;

	va_start(ap, fmt);
	rv = vsnprintf(buf, n, fmt, ap);
	va_end(ap);
	return rv;
}
EXPORT_SYMBOL(snprintf); 
