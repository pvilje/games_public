#pragma once

#include "include/raymath.h"

#define vec2(...) (vec2_t) { __VA_ARGS__ }
typedef Vector2 vec2_t;

#define vec3(...) (vec3_t) { __VA_ARGS__ }
typedef Vector3 vec3_t;

#define mat4(...) (mat4_t) { __VA_ARGS__ }
typedef Matrix mat4_t;

#define vec2FromArray(array) vec2((array)[0], (array)[1])
#define vec3FromArray(array) vec3((array)[0], (array)[1], (array)[2])

static unsigned hashu(unsigned a)
{
    a = (a ^ 61) ^ (a >> 16);
    a = a + (a << 3);
    a = a ^ (a >> 4);
    a = a * 0x27d4eb2d;
    a = a ^ (a >> 15);
    return a;
}

static unsigned hashi(int a)
{
    union { unsigned u; int i; } convert;
    convert.i = a;
    return hashu(convert.u);
}

#define HASH_PRIME0 3323784421u
#define HASH_PRIME1 1449091801u
#define HASH_PRIME2 4280703257u
#define HASH_PRIME3 1609059329u

static unsigned hash2u(unsigned x, unsigned y)
{
    unsigned a = hashu(x);
    unsigned b = hashu(y);

    return (a * HASH_PRIME0) ^ (b * HASH_PRIME1);
}

static unsigned hash3u(unsigned x, unsigned y, unsigned z)
{
    unsigned a = hashu(x);
    unsigned b = hashu(y);
    unsigned c = hashu(z);

    return (a * HASH_PRIME0) ^ (b * HASH_PRIME1) ^ (c * HASH_PRIME2);
}

static unsigned hash4u(unsigned x, unsigned y, unsigned z, unsigned w)
{
    unsigned a = hashu(x);
    unsigned b = hashu(y);
    unsigned c = hashu(z);
    unsigned d = hashu(w);

    return (a * HASH_PRIME0) ^ (b * HASH_PRIME1) ^ (c * HASH_PRIME2) ^ (d * HASH_PRIME3);
}

static unsigned hash2i(int x, int y)
{
    unsigned a = hashi(x);
    unsigned b = hashi(y);

    return (a * HASH_PRIME0) ^ (b * HASH_PRIME1);
}

static unsigned hash3i(int x, int y, int z)
{
    unsigned a = hashi(x);
    unsigned b = hashi(y);
    unsigned c = hashi(z);

    return (a * HASH_PRIME0) ^ (b * HASH_PRIME1) ^ (c * HASH_PRIME2);
}

static unsigned hash4i(int x, int y, int z, int w)
{
    unsigned a = hashi(x);
    unsigned b = hashi(y);
    unsigned c = hashi(z);
    unsigned d = hashi(w);

    return (a * HASH_PRIME0) ^ (b * HASH_PRIME1) ^ (c * HASH_PRIME2) ^ (d * HASH_PRIME3);
}


static unsigned randu(unsigned* state) {
  unsigned x = *state;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  return *state = x;
}

// [min, max)
static int randi(unsigned* state, int min, int max)
{
    return min + randu(state) % (max - min);
}

static float randf(unsigned* state, float min, float max) 
{
    return min + ((float)randu(state) / (float)0xffffffff) * (max - min); 
}

static void randUnit2f(float out[2], unsigned* state)
{
    out[0] = randf(state, -1, 1);
    out[1] = randf(state, -1, 1);
    float d = out[0] * out[0] + out[1] * out[1];
    float k = d != 0? 1 / sqrtf(d) : 0;
    out[0] *= k;
    out[1] *= k;
}

static void randUnit3f(float out[3], unsigned* state)
{
    out[0] = randf(state, -1, 1);
    out[1] = randf(state, -1, 1);
    out[2] = randf(state, -1, 1);
    float d = out[0] * out[0] + out[1] * out[1] + out[2] * out[2];
    float k = d != 0? 1 / sqrtf(d) : 0;
    out[0] *= k;
    out[1] *= k;
    out[2] *= k;
}

static void rand2f(float* out, unsigned* state, float min, float max)
{
    randUnit2f(out, state);
    float scale = randf(state, min, max);
    out[0] *= scale;
    out[1] *= scale;
}

static void rand3f(float* out, unsigned* state, float min, float max)
{
    randUnit3f(out, state);
    float scale = randf(state, min, max);
    out[0] *= scale;
    out[1] *= scale;
    out[2] *= scale;
}

