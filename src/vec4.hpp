#ifndef VEC4_HPP
#define VEC4_HPP

#include <cstdio>
#include <xmmintrin.h>

#if defined(_MSC_VER)
  #define VECCALL __vectorcall
#else
  #define VECCALL
#endif

class alignas(16) vec2
{
public:
    __m128 m;
    
    vec2() : m(_mm_setzero_ps()) { }
    explicit vec2(__m128 v) : m(v) {}
    ~vec2() = default;
    vec2(float x, float y) : m(_mm_setr_ps(x, y, 0.0f, 0.0f)) {}
    
    float x() const { return _mm_cvtss_f32(m); }
    float y() const { return _mm_cvtss_f32(_mm_shuffle_ps(m, m, _MM_SHUFFLE(1,1,1,1))); }
    
    friend vec2 VECCALL operator+(vec2 a, vec2 b) { return vec2(_mm_add_ps(a.m, b.m)); }
    friend vec2 VECCALL operator-(vec2 a, vec2 b) { return vec2(_mm_sub_ps(a.m, b.m)); } 
    friend vec2 VECCALL operator*(vec2 a, vec2 b) { return vec2(_mm_mul_ps(a.m, b.m)); }
    friend vec2 VECCALL operator*(vec2 a, float s) 
    {
        __m128 sv = _mm_setr_ps(s, s, 0.0f, 0.0f);                    
        return vec2(_mm_mul_ps(a.m, sv));
    }
    
    vec2& operator+=(vec2 o) { m = _mm_add_ps(m, o.m); return *this; }
    vec2& operator*=(float s) { *this = *this * s; return *this; }
    
    void print(const char* label = "") const { printf("%s(%.3f, %.3f)\n", label, x(), y()); }
};
#endif