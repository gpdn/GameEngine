#include "mat4.h"

static inline mat4 _mat4_init(float c00, float c01, float c02, float c03, float c10, float c11, float c12, float c13, float c20, float c21, float c22, float c23, float c30, float c31, float c32, float c33) {
    mat4 matrix = {
        .data = {
            {c00, c01, c02, c03},
            {c10, c11, c12, c13},
            {c20, c21, c22, c23},
            {c30, c31, c32, c33}
        }
    };

    return matrix;
}

mat4 mat4_init(float c00, float c01, float c02, float c03, float c10, float c11, float c12, float c13, float c20, float c21, float c22, float c23, float c30, float c31, float c32, float c33) {
    mat4 matrix = {
        .data = {
            {c00, c01, c02, c03},
            {c10, c11, c12, c13},
            {c20, c21, c22, c23},
            {c30, c31, c32, c33}
        }
    };

    return matrix;
}

mat4 mat4_init_vec(const vec4* col0, const vec4* col1, const vec4* col2, const vec4* col3) {
    mat4 matrix = {
        .data = {
            {col0->x, col1->x, col2->x, col3->x},
            {col0->y, col1->y, col2->y, col3->y},
            {col0->z, col1->z, col2->z, col3->z},
            {col0->w, col1->w, col2->w, col3->w}
        }
    };

    return matrix;
}

mat4 mat4_transpose_cp(mat4* mat) {
    mat4 matrix = _mat4_init(
        mat->data[0][0], mat->data[1][0], mat->data[2][0], mat->data[3][0],
        mat->data[0][1], mat->data[1][1], mat->data[2][1], mat->data[3][1],
        mat->data[0][2], mat->data[1][2], mat->data[2][2], mat->data[3][2],
        mat->data[0][3], mat->data[1][3], mat->data[2][3], mat->data[3][3]
    );

    return matrix;
}

void mat4_add(mat4* mat, const mat4* mat2) {
    mat->data[0][0] += mat2->data[0][0];
    mat->data[0][1] += mat2->data[0][1];
    mat->data[0][2] += mat2->data[0][2];
    mat->data[0][3] += mat2->data[0][3];
    mat->data[1][0] += mat2->data[1][0];
    mat->data[1][1] += mat2->data[1][1];
    mat->data[1][2] += mat2->data[1][2];
    mat->data[1][3] += mat2->data[1][3];
    mat->data[2][0] += mat2->data[2][0];
    mat->data[2][1] += mat2->data[2][1];
    mat->data[2][2] += mat2->data[2][2];
    mat->data[2][3] += mat2->data[2][3];
    mat->data[3][0] += mat2->data[3][0];
    mat->data[3][1] += mat2->data[3][1];
    mat->data[3][2] += mat2->data[3][2];
    mat->data[3][3] += mat2->data[3][3];
}

mat4 mat4_add_cp(const mat4* mat, const mat4* mat2) {
    mat4 matrix = mat4_init(
        mat->data[0][0] + mat2->data[0][0],
        mat->data[0][1] + mat2->data[0][1],
        mat->data[0][2] + mat2->data[0][2],
        mat->data[0][3] + mat2->data[0][3],
        mat->data[1][0] + mat2->data[1][0],
        mat->data[1][1] + mat2->data[1][1],
        mat->data[1][2] + mat2->data[1][2],
        mat->data[1][3] + mat2->data[1][3],
        mat->data[2][0] + mat2->data[2][0],
        mat->data[2][1] + mat2->data[2][1],
        mat->data[2][2] + mat2->data[2][2],
        mat->data[2][3] + mat2->data[2][3],
        mat->data[3][0] + mat2->data[3][0],
        mat->data[3][1] + mat2->data[3][1],
        mat->data[3][2] + mat2->data[3][2],
        mat->data[3][3] + mat2->data[3][3]
    );

    return matrix;
}

void mat4_multiply_scalar(mat4* mat, float scalar) {
    mat->data[0][0] *= scalar;
    mat->data[0][1] *= scalar;
    mat->data[0][2] *= scalar;
    mat->data[0][3] *= scalar;
    mat->data[1][0] *= scalar;
    mat->data[1][1] *= scalar;
    mat->data[1][2] *= scalar;
    mat->data[1][3] *= scalar;
    mat->data[2][0] *= scalar;
    mat->data[2][1] *= scalar;
    mat->data[2][2] *= scalar;
    mat->data[2][3] *= scalar;
    mat->data[3][0] *= scalar;
    mat->data[3][1] *= scalar;
    mat->data[3][2] *= scalar;
    mat->data[3][3] *= scalar;
}

mat4 mat4_multiply_scalar_cp(const mat4* mat, float scalar) {
    mat4 matrix = mat4_init(
        mat->data[0][0] * scalar,
        mat->data[0][1] * scalar,
        mat->data[0][2] * scalar,
        mat->data[0][3] * scalar,
        mat->data[1][0] * scalar,
        mat->data[1][1] * scalar,
        mat->data[1][2] * scalar,
        mat->data[1][3] * scalar,
        mat->data[2][0] * scalar,
        mat->data[2][1] * scalar,
        mat->data[2][2] * scalar,
        mat->data[2][3] * scalar,
        mat->data[3][0] * scalar,
        mat->data[3][1] * scalar,
        mat->data[3][2] * scalar,
        mat->data[3][3] * scalar
    );

    return matrix;
}

mat4 mat4_multiply_matrix_cp(const mat4* mat, const mat4* mat2) {
    mat4 matrix = _mat4_init(
        mat->data[0][0] * mat2->data[0][0] + mat->data[0][1] * mat2->data[1][0] + mat->data[0][2] * mat2->data[2][0] + mat->data[0][3] * mat2->data[3][0],
        mat->data[0][0] * mat2->data[0][1] + mat->data[0][1] * mat2->data[1][1] + mat->data[0][2] * mat2->data[2][1] + mat->data[0][3] * mat2->data[3][1],
        mat->data[0][0] * mat2->data[0][2] + mat->data[0][1] * mat2->data[1][2] + mat->data[0][2] * mat2->data[2][2] + mat->data[0][3] * mat2->data[3][2],
        mat->data[0][0] * mat2->data[0][3] + mat->data[0][1] * mat2->data[1][3] + mat->data[0][2] * mat2->data[2][3] + mat->data[0][3] * mat2->data[3][3],

        mat->data[1][0] * mat2->data[0][0] + mat->data[1][1] * mat2->data[1][0] + mat->data[1][2] * mat2->data[2][0] + mat->data[1][3] * mat2->data[3][0],
        mat->data[1][0] * mat2->data[0][1] + mat->data[1][1] * mat2->data[1][1] + mat->data[1][2] * mat2->data[2][1] + mat->data[1][3] * mat2->data[3][1],
        mat->data[1][0] * mat2->data[0][2] + mat->data[1][1] * mat2->data[1][2] + mat->data[1][2] * mat2->data[2][2] + mat->data[1][3] * mat2->data[3][2],
        mat->data[1][0] * mat2->data[0][3] + mat->data[1][1] * mat2->data[1][3] + mat->data[1][2] * mat2->data[2][3] + mat->data[1][3] * mat2->data[3][3],

        mat->data[2][0] * mat2->data[0][0] + mat->data[2][1] * mat2->data[1][0] + mat->data[2][2] * mat2->data[2][0] + mat->data[2][3] * mat2->data[3][0],
        mat->data[2][0] * mat2->data[0][1] + mat->data[2][1] * mat2->data[1][1] + mat->data[2][2] * mat2->data[2][1] + mat->data[2][3] * mat2->data[3][1],
        mat->data[2][0] * mat2->data[0][2] + mat->data[2][1] * mat2->data[1][2] + mat->data[2][2] * mat2->data[2][2] + mat->data[2][3] * mat2->data[3][2],
        mat->data[2][0] * mat2->data[0][3] + mat->data[2][1] * mat2->data[1][3] + mat->data[2][2] * mat2->data[2][3] + mat->data[2][3] * mat2->data[3][3],

        mat->data[3][0] * mat2->data[0][0] + mat->data[3][1] * mat2->data[1][0] + mat->data[3][2] * mat2->data[2][0] + mat->data[3][3] * mat2->data[3][0],
        mat->data[3][0] * mat2->data[0][1] + mat->data[3][1] * mat2->data[1][1] + mat->data[3][2] * mat2->data[2][1] + mat->data[3][3] * mat2->data[3][1],
        mat->data[3][0] * mat2->data[0][2] + mat->data[3][1] * mat2->data[1][2] + mat->data[3][2] * mat2->data[2][2] + mat->data[3][3] * mat2->data[3][2],
        mat->data[3][0] * mat2->data[0][3] + mat->data[3][1] * mat2->data[1][3] + mat->data[3][2] * mat2->data[2][3] + mat->data[3][3] * mat2->data[3][3]
    );

    return matrix;
}

vec4 mat4_multiply_vector_cp(const mat4* mat, const vec4* vec) {
    vec4 vector = {
        mat->data[0][0] * vec->x + mat->data[0][1] * vec->y + mat->data[0][2] * vec->z + mat->data[0][3] * vec->w,
        mat->data[1][0] * vec->x + mat->data[1][1] * vec->y + mat->data[1][2] * vec->z + mat->data[1][3] * vec->w,
        mat->data[2][0] * vec->x + mat->data[2][1] * vec->y + mat->data[2][2] * vec->z + mat->data[2][3] * vec->w,
        mat->data[3][0] * vec->x + mat->data[3][1] * vec->y + mat->data[3][2] * vec->z + mat->data[3][3] * vec->w
    };

    return vector;
}

int mat4_check_symmetric(const mat4* mat) {
    return mat->data[0][1] == mat->data[1][0] &&
           mat->data[0][2] == mat->data[2][0] &&
           mat->data[0][3] == mat->data[3][0] &&
           mat->data[1][2] == mat->data[2][1] &&
           mat->data[1][3] == mat->data[3][1] &&
           mat->data[2][3] == mat->data[3][2];
}

int mat4_check_antisymmetric(const mat4* mat) {
    return mat->data[0][1] == -mat->data[1][0] &&
           mat->data[0][2] == -mat->data[2][0] &&
           mat->data[0][3] == -mat->data[3][0] &&
           mat->data[1][2] == -mat->data[2][1] &&
           mat->data[1][3] == -mat->data[3][1] &&
           mat->data[2][3] == -mat->data[3][2];
}

int mat4_check_diagonal(const mat4* mat) {
    return mat->data[0][1] == 0 &&
           mat->data[0][2] == 0 &&
           mat->data[0][3] == 0 &&
           mat->data[1][0] == 0 &&
           mat->data[1][2] == 0 &&
           mat->data[1][3] == 0 &&
           mat->data[2][0] == 0 &&
           mat->data[2][1] == 0 &&
           mat->data[2][3] == 0 &&
           mat->data[3][0] == 0 &&
           mat->data[3][1] == 0 &&
           mat->data[3][2] == 0;
}

void mat4_print(const mat4* mat) {
    printf("|%f %f %f %f|\n|%f %f %f %f|\n|%f %f %f %f|\n|%f %f %f %f|\n", 
        mat->data[0][0], mat->data[0][1], mat->data[0][2], mat->data[0][3],
        mat->data[1][0], mat->data[1][1], mat->data[1][2], mat->data[1][3],
        mat->data[2][0], mat->data[2][1], mat->data[2][2], mat->data[2][3],
        mat->data[3][0], mat->data[3][1], mat->data[3][2], mat->data[3][3]
    );
}