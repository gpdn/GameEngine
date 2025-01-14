#include "vec4.h"

inline vec4 vec4_init(float x, float y, float z, float w) {
    vec4 vec = {x, y, z, w};
    return vec;
}

inline void vec4_add_scalar(vec4* vec, float scalar) {
    vec->x += scalar;
    vec->y += scalar;
    vec->z += scalar;
    vec->w += scalar;
}

inline vec4 vec4_add_scalar_cp(vec4* vec, float scalar) {
    vec4 new_vec = {vec->x + scalar, vec->y + scalar, vec->z + scalar, vec->w + scalar};
    return new_vec;
}

inline void vec4_multiply_scalar(vec4* vec, float scalar) {
    vec->x *= scalar;
    vec->y *= scalar;
    vec->z *= scalar;
    vec->w *= scalar;
}

inline vec4 vec4_multiply_scalar_cp(vec4* vec, float scalar) {
    vec4 new_vec = {vec->x * scalar, vec->y * scalar, vec->z * scalar, vec->w * scalar};
    return new_vec;
}

inline float vec4_get_magnitude(vec4* vec) {
    return sqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z + vec->w * vec->w);
}

inline vec4 vec4_normalise(vec4* vec) {
    float reciprocal_magnitude = 1.0f/vec4_get_magnitude(vec);
    vec4 new_vec = {vec->x * reciprocal_magnitude, vec->y * reciprocal_magnitude, vec->z * reciprocal_magnitude, vec->w * reciprocal_magnitude};
    return new_vec;
}

inline float vec4_multiply_dot(const vec4* vec, const vec4* vec2) {
    return vec->x * vec2->x + vec->y * vec2->y + vec->z * vec2->z + vec->w * vec2->w; 
}

inline int vec4_check_parallel(const vec4* vec, const vec4* vec2) {
    return abs((int)vec4_multiply_dot(vec, vec2)) == 1;
}

inline int vec4_check_perpendicular(const vec4* vec, const vec4* vec2) {
    return vec4_multiply_dot(vec, vec2) == 0;
}

inline void vec4_print(vec4* vec) {
    printf("x: %f, y: %f, z: %f, w: %f\n", vec->x, vec->y, vec->z, vec->w);
}
