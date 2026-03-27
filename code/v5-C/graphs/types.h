#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

// #ifdef USE_BIG_INT

    // #pragma message "Compiling with 128-bit bitsets"
    typedef __uint128_t bitset_t;
    static inline int bitset_ctz(bitset_t mask) {
        if (mask == 0) return 0; // Or handle as undefined/error

        uint64_t low = (uint64_t)mask;
        if (low != 0) {
            return __builtin_ctzll(low);
        }

        uint64_t high = (uint64_t)(mask >> 64);
        return __builtin_ctzll(high) + 64;
    }
    #define MAX_VERTICES 128

// #else
//
//     // #pragma message "Compiling with 64-bit bitsets"
//
//     typedef uint64_t bitset_t;
//     #define MAX_VERTICES 64
//     static inline int bitset_ctz(bitset_t mask) {
//         return __builtin_ctzll(mask);
//     }
// #endif


#define SIZE(array) (sizeof(array)/sizeof(array[0]))

#define SHIFT(n) (1 << n)
#define SHIFTL(n) (((bitset_t)1) << n)


#define FOR_EACH_BIT(i, mask) \
for (bitset_t _m = (mask); _m; _m &= _m - 1) \
for (int i = bitset_ctz(_m), _once = 1; _once; _once = 0)


#endif //TYPES_H