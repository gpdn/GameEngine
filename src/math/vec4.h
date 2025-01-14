
#ifndef H_VEC_4
#define H_VEC_4

#include <math.h>
#include <stdio.h>

typedef struct vec4 {
    float x;
    float y;
    float z;
    float w;
} vec4;

vec4 vec4_init(float x, float y, float z, float w);
void vec4_add_scalar(vec4* vec, float scalar);
vec4 vec4_add_scalar_cp(vec4* vec, float scalar);
void vec4_multiply_scalar(vec4* vec, float scalar);
vec4 vec4_multiply_scalar_cp(vec4* vec, float scalar);
float vec4_get_magnitude(vec4* vec);
vec4 vec4_normalise(vec4* vec);
float vec4_multiply_dot(const vec4* vec, const vec4* vec2);
int vec4_check_parallel(const vec4* vec, const vec4* vec2);
int vec4_check_perpendicular(const vec4* vec, const vec4* vec2);
void vec4_print(vec4* vec);

#endif