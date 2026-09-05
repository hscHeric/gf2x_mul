#include "gf2x_mul.h"
#include "utils.h"

#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

// Macro para alinhar vetores em endereços multiplos de 128 para uso correto das instruções de load
// das multiplicações de polinómios.h
#define ALIGN_128 __attribute__((aligned(16)))

// #define N (1ULL << 15) // 2^15

#define N 5

int main(void) {
	ALIGN_128 uint64_t a[N];
	ALIGN_128 uint64_t b[N];
	ALIGN_128 uint64_t c[N];

	uint64_t seed = 0x853c49e6748fea9bULL;
	fill_u64(a, N, &seed);
	fill_u64(b, N, &seed);

	int err = gf2x_mul(a, b, c, N);
	if (err != 0) {
		printf("Ocorreu um erro!");
	}

	gf2x_print("C", c, N * 2);

	return 0;
}
