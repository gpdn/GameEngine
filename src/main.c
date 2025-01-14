#include <stdio.h>
#include <stdlib.h>

#include "utils/logger.h"
#include "utils/assertion.h"
#include "math/math.h"
#include "platform/platform.h"
#include "core/application.h"
#include "core/event.h"

int main(int argc, char** argv) {

   /*  vec2 vec = vec2_init(5, 6);

    vec2_print(&vec);

    vec2_add_scalar(&vec, 10);

    vec2_print(&vec);

    float magnitude = vec2_get_magnitude(&vec);

    vec2 unit = vec2_normalise(&vec);

    printf("magnitude: %f\n", magnitude);

    vec2_print(&unit);

    vec4 vector2 = {1, 2, 3, 4};

    vec4_print(&vector2);

    vec4 unit2 = vec4_normalise(&vector2);
    
    vec4_print(&unit2);

    mat3 matrix = mat3_init(
        0, 0, 0,
        0, 0, 0, 
        0, 0, 0
    );
    
    mat3 matrix2 = mat3_init(
        1, 1, 1, 
        2, 2, 1, 
        2, 2, 2
    );

    vec3 v1 = {1, 1, 1};
    vec3 v2 = {2, 2, 2};
    vec3 v3 = {3, 3, 3};

    mat3 mat_vec = mat3_init_vec(&v1, &v2, &v3);

    mat3_print(&matrix2);
    printf("\n");
    mat3_print(&mat_vec);
    printf("\n"); */

    //mat3_multiply_scalar(&mat_vec, 2);
    //mat3_print(&mat_vec);

    //mat3_multiply_matrix(&matrix2, &mat_vec);
    
    //mat3 transp = mat3_transpose_cp(&mat_vec);
    /* mat3_multiply_matrix_cp(&matrix2, &mat_vec);
    mat3_print(&matrix2); */
    /* printf("\n");
    mat3_print(&prod); */

    /* mat3 symm = mat3_init(
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    );

    printf("\n");
    mat3_print(&symm); */

    //LOG_DEBUG("Symmetrical: %d\n", mat3_check_symmetric(&symm));
    //LOG_DEBUG("Antisymmetrical: %d\n", mat3_check_antisymmetric(&symm));
    //LOG_DEBUG("Diagonal: %d\n", mat3_check_diagonal(&symm));

    //E_ASSERT(symm.data[0][0] == 0, "");

    if(!application_init()) return 1;

    if(!application_run()) return 1;

    application_destroy();

    return 0;

}