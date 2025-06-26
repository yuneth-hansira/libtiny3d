#include <stdio.h>
#include "math3d.h"

int main() {
    vec3 cube[8] = {
        {-1,-1,-1}, {1,-1,-1}, {1,1,-1}, {-1,1,-1},
        {-1,-1,1},  {1,-1,1},  {1,1,1},  {-1,1,1}
    };

    mat4 t = mat4_translate(0.0f, 0.0f, 5.0f);
    mat4 s = mat4_scale(0.5f, 0.5f, 0.5f);
    mat4 p = mat4_frustum_asymmetric(-1,1,-1,1,1,10);

    vec3 transformed[8];
    for (int i = 0; i < 8; i++) {
        vec3 v = cube[i];
        v = mat4_mul_vec3(s, v);
        v = mat4_mul_vec3(t, v);
        v = mat4_mul_vec3(p, v);
        transformed[i] = v;
    }

    create_test_pgm("cube.pgm", transformed, 8, 100, 100);
    printf("PGM image generated: cube.pgm\n");

    return 0;
}
