#include <stddef.h>

// borrowed from Linux Kernel
#define container_of(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))


#define container_of_safe(ptr, type, member) ({			\
	char *__mptr = (char *)(ptr);				\
	(size_t)__mptr >= offsetof(type, member) ?		\
	(type *)(__mptr - offsetof(type, member)) : (type *)0; })
