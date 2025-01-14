#ifndef H_MAT_3
#define H_MAT_3

#include <stdio.h>

#include "vec3.h"

typedef struct mat3 {
    float data[3][3];
} mat3;

mat3 mat3_init(float c00, float c01, float c02, float c10, float c11, float c12, float c20, float c21, float c22);
mat3 mat3_init_vec(const vec3* col0, const vec3* col1, const vec3* col2);
mat3 mat3_transpose_cp(mat3* mat);
void mat3_add(mat3* mat, const mat3* mat2);
mat3 mat3_add_cp(const mat3* mat, const mat3* mat2);
void mat3_multiply_scalar(mat3* mat, float scalar);
mat3 mat3_multiply_scalar_cp(const mat3* mat, float scalar);
mat3 mat3_multiply_matrix_cp(const mat3* mat, const mat3* mat2);
vec3 mat3_multiply_vector_cp(const mat3* mat, const vec3* vec);
int mat3_check_symmetric(const mat3* mat);
int mat3_check_antisymmetric(const mat3* mat);
int mat3_check_diagonal(const mat3* mat);
void mat3_print(const mat3* mat);

#endif