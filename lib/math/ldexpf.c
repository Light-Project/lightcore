#include <math.h>
#include <export.h>

float ldexpf(float x, int n)
{
	return scalbnf(x, n);
}
EXPORT_SYMBOL(ldexpf);
