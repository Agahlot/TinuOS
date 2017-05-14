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

char *strcpy(char *dest, const char *src)
{
	size_t i =0 ;
	while ((dest[i] = src[i]) != '\0')
	{
		i++;
	}
	return dest;
}

char *strncpy(char *dest, const char *src, size_t n)
{
	size_t i;
	for (i=0; i < n && src[i] != '\0'; i++)
	{
		dest[i] = src[i];
	}
	/* Just a check */
	for (; i < n; i++)
	{
		dest[i] = '\0';
	}
	return dest;
}