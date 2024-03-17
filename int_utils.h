// https://mail-index.netbsd.org/tech-misc/2007/02/05/0000.html

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define MAX_INT_VAL_STEP(t)					\
	((t) 1 << (CHAR_BIT * sizeof(t) - 1 - ((t) -1 < 1)))
	 
#define MAX_INT_VAL(t) \
	((MAX_INT_VAL_STEP(t) - 1) + MAX_INT_VAL_STEP(t))

#define MIN_INT_VAL(t) \
	((t) -MAX_INT_VAL(t) - 1)

/* uint64_t for 64-bit arch, uint32_t for 32-bit arch */
typedef /*uint32_t*/ size_t my_size_t;

#define MY_SIZE_T_MAX MAX_INT_VAL(my_size_t)
#define OFF_T_MAX MAX_INT_VAL(off_t)
