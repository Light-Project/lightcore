float sqrtf(float x)
{
	__asm__ __volatile__("fsqrt %s0, %s1" : "=w"(x) : "w"(x));
	return x;
}
