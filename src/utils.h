#ifndef UTILS_H
#define UTILS_H

// Gerador pseudoaleatório de 64 bits (SplitMix64)
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static inline uint64_t next_random_u64(uint64_t *state) {
	uint64_t z = (*state += 0x9e3779b97f4a7c15ULL);
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
	z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
	return z ^ (z >> 31);
}

// Função inline para preencher o vetor
static inline void fill_u64(uint64_t *v, size_t n, uint64_t *seed) {
	for (size_t i = 0; i < n; ++i) {
		v[i] = next_random_u64(seed);
	}
}

static inline void gf2x_print(const char *name, const uint64_t *a, size_t n) {
	if (name != NULL)
		printf("%s = ", name); /* Procurar a palavra mais significativa diferente de zero. */
	size_t last = n;
	while (last > 0 && a[last - 1] == 0)
		--last; /* Polinômio zero. */
	if (last == 0) {
		printf("0\n");
		return;
	} /* * Imprime da palavra mais significativa para a menos significativa. * A primeira palavra
	     não precisa de zeros à esquerda. */
	printf("0x%" PRIx64, a[last - 1]);
	for (size_t i = last - 1; i > 0; --i)
		printf("_%016" PRIx64, a[i - 1]);
	printf("\n");
}

#endif // !UTILS_H
