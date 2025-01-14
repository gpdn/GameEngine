#include "vec2.h"

inline vec2 vec2_init(float x, float y) {
    vec2 vec = {x, y};
    return vec;
}

inline void vec2_add_scalar(vec2* vec, float scalar) {
    vec->x += scalar;
    vec->y += scalar;
}

inline vec2 vec2_add_scalar_cp(vec2* vec, float scalar) {
    vec2 new_vec = {vec->x + scalar, vec->y + scalar};
    return new_vec;
}

inline void vec2_multiply_scalar(vec2* vec, float scalar) {
    vec->x *= scalar;
    vec->y *= scalar;
}

inline vec2 vec2_multiply_scalar_cp(vec2* vec, float scalar) {
    vec2 new_vec = {vec->x * scalar, vec->y * scalar};
    return new_vec;
}

inline float vec2_get_magnitude(vec2* vec) {
    return sqrtf(vec->x * vec->x + vec->y * vec->y);
}

inline vec2 vec2_normalise(vec2* vec) {
    float magnitude = vec2_get_magnitude(vec);
    vec2 new_vec = {vec->x / magnitude, vec->y / magnitude};
    return new_vec;
}

inline float vec2_multiply_dot(const vec2* vec, const vec2* vec2) {
    return vec->x * vec2->x + vec->y * vec2->y; 
}

inline int vec2_check_parallel(const vec2* vec, const vec2* vec2) {
    return abs((int)vec2_multiply_dot(vec, vec2)) == 1;
}

inline int vec2_check_perpendicular(const vec2* vec, const vec2* vec2) {
    return vec2_multiply_dot(vec, vec2) == 0;
}

inline void vec2_print(vec2* vec) {
    printf("x: %f, y: %f\n", vec->x, vec->y);
}
