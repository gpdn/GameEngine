
#ifndef H_VEC_3
#define H_VEC_3

#include <math.h>
#include <stdio.h>

typedef struct vec3 {
    float x;
    float y;
    float z;
} vec3;

vec3 vec3_init(float x, float y, float z);
void vec3_add_scalar(vec3* vec, float scalar);
vec3 vec3_add_scalar_cp(vec3* vec, float scalar);
void vec3_multiply_scalar(vec3* vec, float scalar);
vec3 vec3_multiply_scalar_cp(vec3* vec, float scalar);
float vec3_get_magnitude(vec3* vec);
vec3 vec3_normalise(vec3* vec);
float vec3_multiply_dot(const vec3* vec, const vec3* vec2);
int vec3_check_parallel(const vec3* vec, const vec3* vec2);
int vec3_check_perpendicular(const vec3* vec, const vec3* vec2);
void vec3_print(vec3* vec);

#endif