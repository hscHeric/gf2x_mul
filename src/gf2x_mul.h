#ifndef GF2X_MUL_H
#define GF2X_MUL_H

/**
 * @file gf2x_mul.h
 * @brief Multiplicacao de polinomios com Karatsuba.
 */

#include <emmintrin.h>
#include <immintrin.h>
#include <stddef.h>
#include <stdint.h>
#include <wmmintrin.h>

/* Os casos-base recebem vetores alinhados em 16 bytes. */
static inline void karat_mul_2(const uint64_t *a, const uint64_t *b, uint64_t *c) {
	/*
	 * Na memória:     A = [A0, A1]
	 * No registrador: A = [A1, A0]
	 */
	__m128i va = _mm_load_si128((const __m128i *)a); // [A1, A0]
	__m128i vb = _mm_load_si128((const __m128i *)b); // [B1, B0]

	__m128i c0 = _mm_clmulepi64_si128(va, vb, 0x00); // [C0_high, C0_low]
	__m128i c2 = _mm_clmulepi64_si128(va, vb, 0x11); // [C2_high, C2_low]

	__m128i va_high = _mm_srli_si128(va, 8); // [0, A1]
	__m128i vb_high = _mm_srli_si128(vb, 8); // [0, B1]

	__m128i sa = _mm_xor_si128(va, va_high); // [A1, A0 ^ A1]
	__m128i sb = _mm_xor_si128(vb, vb_high); // [B1, B0 ^ B1]

	__m128i cm = _mm_clmulepi64_si128(sa, sb, 0x00); // [CM_high, CM_low]

	/* C1 = CM + C0 + C2. */
	__m128i c1 = _mm_xor_si128(cm, c0);
	c1 = _mm_xor_si128(c1, c2); // [C1_high, C1_low]

	__m128i c_low = _mm_xor_si128(c0, _mm_slli_si128(c1, 8));  // [C0_high ^ C1_low, C0_low]
	__m128i c_high = _mm_xor_si128(c2, _mm_srli_si128(c1, 8)); // [C2_high, C2_low ^ C1_high]

	_mm_store_si128((__m128i *)&c[0], c_low);  // c[0], c[1]
	_mm_store_si128((__m128i *)&c[2], c_high); // c[2], c[3]
}

static inline void karat_mul_3(const uint64_t *a, const uint64_t *b, uint64_t *c) {

	/*
	 * Cada +1 desloca o endereco em 8 bytes. Por isso, a e a + 1 nao
	 * podem estar alinhados em 16 bytes ao mesmo tempo: use load no
	 * endereco multiplo de 16 e loadu no outro.
	 */
	__m128i va0 = _mm_load_si128((const __m128i *)a);        // [A1, A0]
	__m128i va1 = _mm_loadu_si128((const __m128i *)(a + 1)); // [A2, A1]

	__m128i vb0 = _mm_load_si128((const __m128i *)b);        // [B1, B0]
	__m128i vb1 = _mm_loadu_si128((const __m128i *)(b + 1)); // [B2, B1]

	// R0 = C0
	__m128i r0 = _mm_clmulepi64_si128(va0, vb0, 0x00); // A0 * B0 (C0)

	__m128i r1 = _mm_clmulepi64_si128(va1, vb1, 0x00); // A1 * B1

	// C4 = R2
	__m128i r2 = _mm_clmulepi64_si128(va1, vb1, 0x11); // A2 * B2

	__m128i sa = _mm_xor_si128(va0, va1); // [A2 + A1, A1 + A0]
	__m128i sb = _mm_xor_si128(vb0, vb1); // [B2 + B1, B1 + B0]

	__m128i r3 = _mm_clmulepi64_si128(sa, sb, 0x00); // (A0 + A1) * (B0 + B1);
	__m128i r5 = _mm_clmulepi64_si128(sa, sb, 0x11); // (A1 + A2) * (B1 + B2);

	sa = _mm_xor_si128(va0, _mm_srli_si128(va1, 8)); // [A1, A2 + A0]
	sb = _mm_xor_si128(vb0, _mm_srli_si128(vb1, 8)); // [B1, B2 + B0]
	__m128i r4 = _mm_clmulepi64_si128(sa, sb, 0x00); // (A0 + A2) * (B0 * B2);

	// C1 = R3 + R0 + R1
	__m128i c1 = _mm_xor_si128(r3, r0);
	c1 = _mm_xor_si128(c1, r1);

	// C2 = R4 + R0 + R1 + R2
	__m128i c2 = _mm_xor_si128(r4, r0);
	c2 = _mm_xor_si128(c2, r1);
	c2 = _mm_xor_si128(c2, r2);

	// C3 = R5 + R1 + R2
	__m128i c3 = _mm_xor_si128(r5, r1);
	c3 = _mm_xor_si128(c3, r2);

	__m128i out01 = _mm_xor_si128(r0, _mm_slli_si128(c1, 8));
	__m128i out23 = _mm_xor_si128(_mm_srli_si128(c1, 8), c2);
	out23 = _mm_xor_si128(out23, _mm_slli_si128(c3, 8));
	__m128i out45 = _mm_xor_si128(_mm_srli_si128(c3, 8), r2);
	_mm_store_si128((__m128i *)&c[0], out01);
	_mm_store_si128((__m128i *)&c[2], out23);
	_mm_store_si128((__m128i *)&c[4], out45);
}

static inline void karat_mul_5(const uint64_t *a, const uint64_t *b, uint64_t *c) {
	// TODO: Implementar 5 Karatsuba
	/*
	 * a + 2 e b + 2 avancam 16 bytes e mantem o mesmo alinhamento dos
	 * enderecos iniciais. A ultima palavra e carregada separadamente para
	 * nao acessar uma sexta palavra fora do vetor.
	 */
	__m128i va0, va1, va2;
	__m128i vb0, vb1, vb2;
	__m128i sa, sb;
	__m128i va_tmp, vb_tmp;
	__m128i r10, r6, r14, r12, r0, r1, r2, r3, r4, r5, r9, r7, r11, r8, r13;
	__m128i c1, c2, c3, c4, c5, c6, c7;

	va0 = _mm_load_si128((const __m128i *)a);        // [A1, A0]
	va1 = _mm_load_si128((const __m128i *)(a + 2));  // [A3, A2]
	va2 = _mm_loadu_si128((const __m128i *)(a + 3)); // [A4, A3]

	vb0 = _mm_load_si128((const __m128i *)b);        // [B1, B0]
	vb1 = _mm_load_si128((const __m128i *)(b + 2));  // [B3, B2]
	vb2 = _mm_loadu_si128((const __m128i *)(b + 3)); // [B4, B3]

	r0 = _mm_clmulepi64_si128(va0, vb0, 0x00); // (A0 * B0)
	r1 = _mm_clmulepi64_si128(va0, vb0, 0x11); // (A1 * B1)
	r2 = _mm_clmulepi64_si128(va1, vb1, 0x00); // (A2 * B2)
	r3 = _mm_clmulepi64_si128(va2, vb2, 0x00); // (A3 * B3)
	r4 = _mm_clmulepi64_si128(va2, vb2, 0x11); // (A4 * B4)

	sa = _mm_xor_si128(va0, va1); // [A1 + A3, A0 + A2]
	sb = _mm_xor_si128(vb0, vb1); // [B1 + B3, A0 + A2]

	r10 = _mm_clmulepi64_si128(sa, sb, 0x11); // (A1 + A3) * (B1 + B3)
	r6 = _mm_clmulepi64_si128(sa, sb, 0x00);  // (A0 + A2) * (B0 + B2)

	sa = _mm_xor_si128(va1, va2); // [A3 + A4, A2 + A3]
	sb = _mm_xor_si128(vb1, vb2); // [B3 + B4, B2 + B3]

	r14 = _mm_clmulepi64_si128(sa, sb, 0x11); // (A3 + A4) * (B3 + B4)
	r12 = _mm_clmulepi64_si128(sa, sb, 0x00); // (A2 + A3) * (B2 + B3)

	va_tmp = _mm_srli_si128(va0, 8); // [0, A1]
	vb_tmp = _mm_srli_si128(vb0, 8); // [0, B1]

	sa = _mm_xor_si128(va0, va_tmp);         // [A1, A0 + A1]
	sb = _mm_xor_si128(vb0, vb_tmp);         // [B1, B0 + B1]
	r5 = _mm_clmulepi64_si128(sa, sb, 0x00); // (A0 + A1) * (B0 + B1)

	sa = _mm_xor_si128(va1, va_tmp); // [A3, A2 + A1]
	sb = _mm_xor_si128(vb1, vb_tmp); // [B3, B2 + B1]
	r9 = _mm_clmulepi64_si128(sa, sb, 0x00);

	sa = _mm_xor_si128(va0, va2);             // [A1 + A4, A0 + A3]
	sb = _mm_xor_si128(vb0, vb2);             // [B1 + B4, B0 + B3]
	r7 = _mm_clmulepi64_si128(sa, sb, 0x00);  // (A0 + A3) * (B0 + B3)
	r11 = _mm_clmulepi64_si128(sa, sb, 0x11); // (A1 + A4) * (B1 + B4)

	va_tmp = _mm_srli_si128(va2, 8);         // [0, A4]
	vb_tmp = _mm_srli_si128(vb2, 8);         // [0, A4]
	sa = _mm_xor_si128(va0, va_tmp);         // [A1, A0 + A4]
	sb = _mm_xor_si128(vb0, vb_tmp);         // [B1, B0 + B4]
	r8 = _mm_clmulepi64_si128(sa, sb, 0x00); // (A0 + A4) * (B0 + B4)

	sa = _mm_xor_si128(va1, va_tmp);          // [A3, A2 + A4]
	sb = _mm_xor_si128(vb1, vb_tmp);          // [B3, B2 + B4]
	r13 = _mm_clmulepi64_si128(sa, sb, 0x00); // (A2 + A4) * (B2 + B4)

	// Reaproveitamento das somas. A soma écomultatia (A + B) + C = A + (B + C)
	__m128i t01 = _mm_xor_si128(r0, r1);
	__m128i t23 = _mm_xor_si128(r2, r3);
	__m128i t24 = _mm_xor_si128(r2, r4);
	__m128i t34 = _mm_xor_si128(r3, r4);

	__m128i t012 = _mm_xor_si128(t01, r2);
	__m128i t0123 = _mm_xor_si128(t01, t23);
	__m128i t01234 = _mm_xor_si128(t0123, r4);

	// C0 = R0

	// C1 = R5 + R0 + R1
	c1 = _mm_xor_si128(r5, t01);

	// C2 = R1 + R6 + R0 + R2
	c2 = _mm_xor_si128(r6, t012);

	// C3 = R7 + R0 + R3 + R9 + R1 + R2
	c3 = _mm_xor_si128(_mm_xor_si128(r7, r9), t0123);

	// C4 = R2 + R8 + R0 + R4 + R10 + R1 + R3
	c4 = _mm_xor_si128(_mm_xor_si128(r8, r10), t01234);

	// C5 = R11 + R1 + R4 + R12 + R2 + R3
	c5 = _mm_xor_si128(_mm_xor_si128(r11, r12), _mm_xor_si128(r1, t24));

	// C6 ← R3 + R13 + R2 + R4
	c6 = _mm_xor_si128(r13, _mm_xor_si128(t23, r4));

	// C7 = R14 + R3 + R4
	c7 = _mm_xor_si128(r14, t34);

	// C8 = R4

	// C0 = R0
	__m128i out01 = _mm_xor_si128(r0, _mm_slli_si128(c1, 8));

	__m128i out23 = _mm_xor_si128(_mm_srli_si128(c1, 8), c2);
	out23 = _mm_xor_si128(out23, _mm_slli_si128(c3, 8));

	__m128i out45 = _mm_xor_si128(_mm_srli_si128(c3, 8), c4);
	out45 = _mm_xor_si128(out45, _mm_slli_si128(c5, 8));

	__m128i out67 = _mm_xor_si128(_mm_srli_si128(c5, 8), c6);
	out67 = _mm_xor_si128(out67, _mm_slli_si128(c7, 8));

	__m128i out89 = _mm_xor_si128(_mm_srli_si128(c7, 8), r4);

	_mm_store_si128((__m128i *)&c[0], out01);
	_mm_store_si128((__m128i *)&c[2], out23);
	_mm_store_si128((__m128i *)&c[4], out45);
	_mm_store_si128((__m128i *)&c[6], out67);
	_mm_store_si128((__m128i *)&c[8], out89);
}

static inline void mul_base_1(const uint64_t *a, const uint64_t *b, uint64_t *c) {
	const __m128i va = _mm_set_epi64x(0, (long long)a[0]);
	const __m128i vb = _mm_set_epi64x(0, (long long)b[0]);
	const __m128i product = _mm_clmulepi64_si128(va, vb, 0x00);
	_mm_storeu_si128((__m128i *)(void *)c, product);
}

static inline int gf2x_mul(uint64_t *a, uint64_t *b, uint64_t *c, size_t n) {
	// TODO: Implementar a decomposição do polinômio nos sub-casos e fazer a composição do resultado
	return 1;
}

#endif
