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

int memcmp(const void *s1, const void *s2, size_t n)
{
	unsigned char u1, u2;

	for ( ; n-- ; s1++, s2++)
	{
		u1 = * (unsigned char *)s1;
		u2 = * (unsigned char *)s2;
		if (u1 != u2)
		{
			return (u1 - u2);
		}
	}
	return 0;
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