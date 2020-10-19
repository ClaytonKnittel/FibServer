#ifndef __UTIL_H
#define __UTIL_H

#include <assert.h>
#include <stdint.h>
#include <stdio.h>


typedef uint64_t ptr_int_t;


// true if size is aligned by n, otherwise false
#define IS_ALIGNED(size, n) ((((uint64_t) (size)) & ((n) - 1)) == 0lu)

// round up to the nearest n, assuming n is a power of 2
#define ALIGN_UP(size, n) ((((uint64_t)size) + ((n) - 1)) & ~((n) - 1))

// round down to the nearest n, assuming n is a power of 2
#define ALIGN_DOWN(size, n) ((uint64_t)(size) & ~((n) - 1))

#define PAGE_ALIGN(size) ALIGN_UP(size, PAGE_SIZE)


#define ALIGN_CACHE_LINE \
    char __trash[0] __attribute__ ((aligned(CACHE_LINE_SIZE)))

#define ptr_less(ptr1, ptr2) \
    (((ptr_int_t) (ptr1)) < ((ptr_int_t) (ptr2)))

#define is_pow2(n) \
    ((((uint64_t) (n)) & (((uint64_t) n) - 1)) == 0)


#define P_FILE_LINE P_LGREEN __FILE__ P_DEFAULT ":" P_LCYAN "%d" P_DEFAULT

#ifdef DEBUG

#define FIB_PRINTEXPR(expr) \
    fprintf(stderr, P_FILE_LINE " " P_LRED "assert " P_LYELLOW "\"" \
            #expr "\"" P_LRED " failed" P_RESET "\n", __LINE__);


#define FIB_ASSERT(expr) \
    do { \
        if (__builtin_expect(!(expr), 0)) { \
            fprintf(stderr, P_FILE_LINE " " P_LRED "assert " P_LYELLOW "\"" \
                    #expr "\"" P_LRED " failed (tid %lu)" P_RESET "\n", \
                    __LINE__, pthread_self()); \
            assert(0); \
        } \
    } while (0)
#else
#define FIB_ASSERT(expr)
#endif


#endif /* __UTIL_H */
