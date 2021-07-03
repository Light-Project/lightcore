#include <math.h>
#include <export.h>

double ldexp(double x, int n)
{
	return scalbn(x, n);
}
EXPORT_SYMBOL(ldexp);
