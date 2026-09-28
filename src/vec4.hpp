#ifndef VEC2_HPP
#define VEC2_HPP

#include <cstdio>
#include <cstdint>
#include <smmintrin.h>   // SSE4.1 (inclut SSE2)

#if defined(_MSC_VER)
  #define VECCALL __vectorcall
#else
  #define VECCALL
#endif

class alignas(16) vec2
{
public:
    __m128i m;

    vec2() : m(_mm_setzero_si128()) {}
    explicit vec2(__m128i v) : m(v) {}
    vec2(int32_t x, int32_t y) : m(_mm_setr_epi32(x, y, 0, 0)) {}

    int32_t x() const { return _mm_cvtsi128_si32(m); }
    int32_t y() const { return _mm_extract_epi32(m, 1); }

    friend vec2 VECCALL operator+(vec2 a, vec2 b) { return vec2(_mm_add_epi32(a.m, b.m)); }
    friend vec2 VECCALL operator-(vec2 a, vec2 b) { return vec2(_mm_sub_epi32(a.m, b.m)); }
    friend vec2 VECCALL operator*(vec2 a, vec2 b) { return vec2(_mm_mullo_epi32(a.m, b.m)); }
    friend vec2 VECCALL operator*(vec2 a, int32_t s) { return vec2(_mm_mullo_epi32(a.m, _mm_set1_epi32(s))); }

    vec2& operator+=(vec2 o) { m = _mm_add_epi32(m, o.m); return *this; }
    vec2& operator-=(vec2 o) { m = _mm_sub_epi32(m, o.m); return *this; }
    vec2& operator*=(int32_t s) { m = _mm_mullo_epi32(m, _mm_set1_epi32(s)); return *this; }

    void print(const char* label = "") const
    {
        printf("%s(%d, %d)\n", label, (int)x(), (int)y());
    }
};
#endif