#ifndef H_MAT_4
#define H_MAT_4

#include <stdio.h>

#include "vec4.h"

typedef struct mat4 {
    float data[4][4];
} mat4;

mat4 mat4_init(float c00, float c01, float c02, float c03, float c10, float c11, float c12, float c13, float c20, float c21, float c22, float c23, float c30, float c31, float c32, float c33);
mat4 mat4_init_vec(const vec4* col0, const vec4* col1, const vec4* col2, const vec4* col3);
mat4 mat4_transpose_cp(mat4* mat);
void mat4_add(mat4* mat, const mat4* mat2);
mat4 mat4_add_cp(const mat4* mat, const mat4* mat2);
void mat4_multiply_scalar(mat4* mat, float scalar);
mat4 mat4_multiply_scalar_cp(const mat4* mat, float scalar);
mat4 mat4_multiply_matrix_cp(const mat4* mat, const mat4* mat2);
vec4 mat4_multiply_vector_cp(const mat4* mat, const vec4* vec);
int mat4_check_symmetric(const mat4* mat);
int mat4_check_antisymmetric(const mat4* mat);
int mat4_check_diagonal(const mat4* mat);
void mat4_print(const mat4* mat);

#endif