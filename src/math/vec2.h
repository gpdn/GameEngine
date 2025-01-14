
#ifndef H_VEC_2
#define H_VEC_2

#include <math.h>
#include <stdio.h>

typedef struct vec2 {
    float x;
    float y;
} vec2;

vec2 vec2_init(float x, float y);
void vec2_add_scalar(vec2* vec, float scalar);
vec2 vec2_add_scalar_cp(vec2* vec, float scalar);
void vec2_multiply_scalar(vec2* vec, float scalar);
vec2 vec2_multiply_scalar_cp(vec2* vec, float scalar);
float vec2_get_magnitude(vec2* vec);
vec2 vec2_normalise(vec2* vec);
float vec2_multiply_dot(const vec2* vec, const vec2* vec2);
int vec2_check_parallel(const vec2* vec, const vec2* vec2);
int vec2_check_perpendicular(const vec2* vec, const vec2* vec2);
void vec2_print(vec2* vec);

#endif