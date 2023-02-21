#ifndef PGC_HPP_
#define PGC_HPP_

#include <cstdint>  // uint64_t, uint32_t

// https://www.pcg-random.org/

typedef struct {

    uint64_t state;
    uint64_t inc;

} pcg32_random_t;

inline auto pcg32_random_r(pcg32_random_t &rng) {

    uint64_t oldstate = rng.state;
    // Advance internal state
    rng.state = oldstate * 6364136223846793005ULL + (rng.inc|1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

inline auto pcg32_srandom_r(uint64_t initstate, uint64_t initseq) {

    pcg32_random_t rng;
    rng.state = 0U;
    rng.inc = (initseq << 1u) | 1u;
    pcg32_random_r(rng);
    rng.state += initstate;
    pcg32_random_r(rng);
    return rng;
}

#endif
