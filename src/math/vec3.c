#include "vec3.h"

inline vec3 vec3_init(float x, float y, float z) {
    vec3 vec = {x, y, z};
    return vec;
}

inline void vec3_add_scalar(vec3* vec, float scalar) {
    vec->x += scalar;
    vec->y += scalar;
    vec->z += scalar;
}

inline vec3 vec3_add_scalar_cp(vec3* vec, float scalar) {
    vec3 new_vec = {vec->x + scalar, vec->y + scalar, vec->z + scalar};
    return new_vec;
}

inline void vec3_multiply_scalar(vec3* vec, float scalar) {
    vec->x *= scalar;
    vec->y *= scalar;
    vec->z *= scalar;
}

inline vec3 vec3_multiply_scalar_cp(vec3* vec, float scalar) {
    vec3 new_vec = {vec->x * scalar, vec->y * scalar, vec->z * scalar};
    return new_vec;
}

inline float vec3_get_magnitude(vec3* vec) {
    return sqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
}

inline vec3 vec3_normalise(vec3* vec) {
    float reciprocal_magnitude = 1.0f/vec3_get_magnitude(vec);
    vec3 new_vec = {vec->x * reciprocal_magnitude, vec->y * reciprocal_magnitude, vec->z * reciprocal_magnitude};
    return new_vec;
}

inline float vec3_multiply_dot(const vec3* vec, const vec3* vec2) {
    return vec->x * vec2->x + vec->y * vec2->y + vec->z * vec2->z; 
}

inline int vec3_check_parallel(const vec3* vec, const vec3* vec2) {
    return abs((int)vec3_multiply_dot(vec, vec2)) == 1;
}

inline int vec3_check_perpendicular(const vec3* vec, const vec3* vec2) {
    return vec3_multiply_dot(vec, vec2) == 0;
}


inline void vec3_print(vec3* vec) {
    printf("x: %f, y: %f, z: %f\n", vec->x, vec->y, vec->z);
}
