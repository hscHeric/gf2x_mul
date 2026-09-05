#ifndef UTILS_H
#define UTILS_H

// Gerador pseudoaleatório de 64 bits (SplitMix64)
#include <stddef.h>
#include <stdint.h>

static inline uint64_t next_random_u64(uint64_t *state) {
	uint64_t z = (*state += 0x9e3779b97f4a7c15ULL);
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
	z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
	return z ^ (z >> 31);
}

// Função inline para preencher o vetor
inline void fill_u64(uint64_t *v, size_t n, uint64_t *seed) {
	for (size_t i = 0; i < n; ++i) {
		v[i] = next_random_u64(seed);
	}
}

#endif // !UTILS_H
