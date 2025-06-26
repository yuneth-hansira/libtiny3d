#ifndef MATH3D_H
#define MATH3D_H

// 3D vector with both Cartesian and spherical coordinates
typedef union {
    struct { float x, y, z; };
    struct { float r, theta, phi; };
} vec3;

// 4x4 matrix, column-major
typedef struct {
    float m[16];
} mat4;

// --- vec3 functions ---
vec3 vec3_from_spherical(float r, float theta, float phi);
vec3 vec3_normalize_fast(vec3 v);
vec3 vec3_slerp(vec3 a, vec3 b, float t);

// --- mat4 functions ---
mat4 mat4_identity();
mat4 mat4_translate(float tx, float ty, float tz);
mat4 mat4_scale(float sx, float sy, float sz);
mat4 mat4_rotate_xyz(float rx, float ry, float rz);
mat4 mat4_frustum_asymmetric(float l, float r, float b, float t, float n, float f);

// --- matrix-vector ops ---
vec3 mat4_mul_vec3(mat4 m, vec3 v);

// --- visualization ---
void create_test_pgm(const char *filename, vec3 *points, int point_count, int width, int height);
void draw_wireframe_pgm(const char *filename, vec3 *points, int point_count, const int edges[][2], int edge_count, int width, int height);

#endif // MATH3D_H
