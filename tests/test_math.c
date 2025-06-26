#include <stdio.h>
#include "math3d.h"

int main() {
    vec3 cube[8] = {
        {-1,-1,-1}, {1,-1,-1}, {1,1,-1}, {-1,1,-1},
        {-1,-1,1},  {1,-1,1},  {1,1,1},  {-1,1,1}
    };

    // Cube edges (pairs of indices into cube array)
    const int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0}, // bottom
        {4,5},{5,6},{6,7},{7,4}, // top
        {0,4},{1,5},{2,6},{3,7}  // sides
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

    draw_wireframe_pgm("cube_wireframe.pgm", transformed, 8, edges, 12, 100, 100);
    printf("PGM wireframe image generated: cube_wireframe.pgm\n");

    return 0;
}
