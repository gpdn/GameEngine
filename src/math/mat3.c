#include "mat3.h"

static inline mat3 _mat3_init(float c00, float c01, float c02, float c10, float c11, float c12, float c20, float c21, float c22) {
    mat3 matrix = {
        .data = {
            {c00, c01, c02},
            {c10, c11, c12},
            {c20, c21, c22}
        }
    };

    return matrix;
}

inline mat3 mat3_init(float c00, float c01, float c02, float c10, float c11, float c12, float c20, float c21, float c22) {
    mat3 matrix = {
        .data = {
            {c00, c10, c20},
            {c01, c11, c21},
            {c02, c12, c22}
        }
    };

    return matrix;
}

inline mat3 mat3_init_vec(const vec3* col0, const vec3* col1, const vec3* col2) {
    mat3 matrix = {
        .data = {
            {col0->x, col1->x, col2->x},
            {col0->y, col1->y, col2->y},
            {col0->z, col1->z, col2->z}
        }
    };

    return matrix;
}

mat3 mat3_transpose_cp(mat3* mat) {
    
    mat3 matrix = _mat3_init(
        mat->data[0][0], mat->data[1][0], mat->data[2][0],
        mat->data[0][1], mat->data[1][1], mat->data[2][1],
        mat->data[0][2], mat->data[1][2], mat->data[2][2]
    );

    return matrix;
}

void mat3_add(mat3* mat, const mat3* mat2) {
    mat->data[0][0] += mat2->data[0][0];
    mat->data[0][1] += mat2->data[0][1];
    mat->data[0][2] += mat2->data[0][2];
    mat->data[1][0] += mat2->data[1][0];
    mat->data[1][1] += mat2->data[1][1];
    mat->data[1][2] += mat2->data[1][2];
    mat->data[2][0] += mat2->data[2][0];
    mat->data[2][1] += mat2->data[2][1];
    mat->data[2][2] += mat2->data[2][2];
}

mat3 mat3_add_cp(const mat3* mat, const mat3* mat2) {
    mat3 matrix = mat3_init(
        mat->data[0][0] + mat2->data[0][0], 
        mat->data[0][1] + mat2->data[0][1], 
        mat->data[0][2] + mat2->data[0][2], 
        mat->data[1][0] + mat2->data[1][0], 
        mat->data[1][1] + mat2->data[1][1], 
        mat->data[1][2] + mat2->data[1][2], 
        mat->data[2][0] + mat2->data[2][0], 
        mat->data[2][1] + mat2->data[2][1], 
        mat->data[2][2] + mat2->data[2][2]
    );

    return matrix;
}

void mat3_multiply_scalar(mat3* mat, float scalar) {
    mat->data[0][0] *= scalar; 
    mat->data[0][1] *= scalar; 
    mat->data[0][2] *= scalar; 
    mat->data[1][0] *= scalar; 
    mat->data[1][1] *= scalar; 
    mat->data[1][2] *= scalar; 
    mat->data[2][0] *= scalar; 
    mat->data[2][1] *= scalar; 
    mat->data[2][2] *= scalar; 
}

mat3 mat3_multiply_scalar_cp(const mat3* mat, float scalar) {
    mat3 matrix = mat3_init(
        mat->data[0][0] * scalar, 
        mat->data[0][1] * scalar, 
        mat->data[0][2] * scalar, 
        mat->data[1][0] * scalar, 
        mat->data[1][1] * scalar, 
        mat->data[1][2] * scalar, 
        mat->data[2][0] * scalar, 
        mat->data[2][1] * scalar, 
        mat->data[2][2] * scalar
    );

    return matrix;
}

mat3 mat3_multiply_matrix_cp(const mat3* mat, const mat3* mat2) {
    mat3 matrix = _mat3_init(
        mat->data[0][0] * mat2->data[0][0] + mat->data[0][1] * mat2->data[1][0] + mat->data[0][2] * mat2->data[2][0],
        mat->data[0][0] * mat2->data[0][1] + mat->data[0][1] * mat2->data[1][1] + mat->data[0][2] * mat2->data[2][1],
        mat->data[0][0] * mat2->data[0][2] + mat->data[0][1] * mat2->data[1][2] + mat->data[0][2] * mat2->data[2][2],

        mat->data[1][0] * mat2->data[0][0] + mat->data[1][1] * mat2->data[1][0] + mat->data[1][2] * mat2->data[2][0],
        mat->data[1][0] * mat2->data[0][1] + mat->data[1][1] * mat2->data[1][1] + mat->data[1][2] * mat2->data[2][1],
        mat->data[1][0] * mat2->data[0][2] + mat->data[1][1] * mat2->data[1][2] + mat->data[1][2] * mat2->data[2][2],

        mat->data[2][0] * mat2->data[0][0] + mat->data[2][1] * mat2->data[1][0] + mat->data[2][2] * mat2->data[2][0],
        mat->data[2][0] * mat2->data[0][1] + mat->data[2][1] * mat2->data[1][1] + mat->data[2][2] * mat2->data[2][1],
        mat->data[2][0] * mat2->data[0][2] + mat->data[2][1] * mat2->data[1][2] + mat->data[2][2] * mat2->data[2][2]
    );

    return matrix;
}

vec3 mat3_multiply_vector_cp(const mat3* mat, const vec3* vec) {
    vec3 vector = {
        mat->data[0][0] * vec->x + mat->data[0][1] * vec->y + mat->data[0][2] * vec->z,
        mat->data[1][0] * vec->x + mat->data[1][1] * vec->y + mat->data[1][2] * vec->z,
        mat->data[2][0] * vec->x + mat->data[2][1] * vec->y + mat->data[2][2] * vec->z
    };

    return vector;
}

int mat3_check_symmetric(const mat3* mat) {
    return mat->data[0][1] == mat->data[1][0] &&
           mat->data[0][2] == mat->data[2][0] &&
           mat->data[1][2] == mat->data[2][1];
}

int mat3_check_antisymmetric(const mat3* mat) {
    return mat->data[0][1] == -mat->data[1][0] &&
           mat->data[0][2] == -mat->data[2][0] &&
           mat->data[1][2] == -mat->data[2][1];
}

int mat3_check_diagonal(const mat3* mat) {
    return mat->data[0][1] == 0 &&
           mat->data[0][2] == 0 &&
           mat->data[1][0] == 0 &&
           mat->data[1][2] == 0 &&
           mat->data[2][0] == 0 &&
           mat->data[2][1] == 0;
}

void mat3_print(const mat3* mat) {
    printf("|%f %f %f|\n|%f %f %f|\n|%f %f %f|\n", 
        mat->data[0][0], mat->data[0][1], mat->data[0][2],
        mat->data[1][0], mat->data[1][1], mat->data[1][2],
        mat->data[2][0], mat->data[2][1], mat->data[2][2]
    );
}