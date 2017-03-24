#include <boot.h>
#include <types.h>

void *memcpy(void *dest, const void *src, size_t n)
{
	asm volatile("cld; rep movsb" : "+c" (n), "+S" (src), "+D" (dest) : : "memory");
	return dest;
}

void *memset(void * b, int val, size_t count) {
	asm volatile ("cld; rep stosb" : "+c" (count), "+D" (b) : "a" (val) : "memory");
	return b;
}
