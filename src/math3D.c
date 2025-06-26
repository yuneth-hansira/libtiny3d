#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "math3d.h"

// --- vec3 functions ---

// Convert spherical (r, theta, phi) to Cartesian (x, y, z)
vec3 vec3_from_spherical(float r, float theta, float phi) {
    vec3 v;
    v.x = r * sinf(theta) * cosf(phi);
    v.y = r * sinf(theta) * sinf(phi);
    v.z = r * cosf(theta);
    return v;
}

// Fast normalize using Quake's fast inverse square root
vec3 vec3_normalize_fast(vec3 v) {
    float x = v.x, y = v.y, z = v.z;
    float len_sq = x*x + y*y + z*z;
    float x2 = len_sq * 0.5f;
    float threehalfs = 1.5f;
    union { float f; unsigned int i; } conv = { len_sq };
    conv.i = 0x5f3759df - (conv.i >> 1);
    float inv_sqrt = conv.f * (threehalfs - (x2 * conv.f * conv.f));
    v.x *= inv_sqrt;
    v.y *= inv_sqrt;
    v.z *= inv_sqrt;
    return v;
}

// Spherical linear interpolation between two vectors
vec3 vec3_slerp(vec3 a, vec3 b, float t) {
    a = vec3_normalize_fast(a);
    b = vec3_normalize_fast(b);
    float dot = a.x*b.x + a.y*b.y + a.z*b.z;
    if (dot > 0.9995f) {
        vec3 result = {
            a.x + t*(b.x - a.x),
            a.y + t*(b.y - a.y),
            a.z + t*(b.z - a.z)
        };
        return vec3_normalize_fast(result);
    }
    if (dot < -1.0f) dot = -1.0f;
    if (dot >  1.0f) dot =  1.0f;
    float theta = acosf(dot) * t;
    vec3 rel = { b.x - a.x*dot, b.y - a.y*dot, b.z - a.z*dot };
    rel = vec3_normalize_fast(rel);
    vec3 result = {
        a.x * cosf(theta) + rel.x * sinf(theta),
        a.y * cosf(theta) + rel.y * sinf(theta),
        a.z * cosf(theta) + rel.z * sinf(theta)
    };
    return result;
}

// --- mat4 functions ---

mat4 mat4_identity() {
    mat4 m = {0};
    m.m[0] = m.m[5] = m.m[10] = m.m[15] = 1.0f;
    return m;
}

mat4 mat4_translate(float tx, float ty, float tz) {
    mat4 m = mat4_identity();
    m.m[12] = tx;
    m.m[13] = ty;
    m.m[14] = tz;
    return m;
}

mat4 mat4_scale(float sx, float sy, float sz) {
    mat4 m = mat4_identity();
    m.m[0] = sx;
    m.m[5] = sy;
    m.m[10] = sz;
    return m;
}

// Euler XYZ rotation (in radians)
mat4 mat4_rotate_xyz(float rx, float ry, float rz) {
    float cx = cosf(rx), sx = sinf(rx);
    float cy = cosf(ry), sy = sinf(ry);
    float cz = cosf(rz), sz = sinf(rz);

    mat4 mx = mat4_identity();
    mx.m[5] = cx;  mx.m[6] = -sx;
    mx.m[9] = sx;  mx.m[10] = cx;

    mat4 my = mat4_identity();
    my.m[0] = cy;  my.m[2] = sy;
    my.m[8] = -sy; my.m[10] = cy;

    mat4 mz = mat4_identity();
    mz.m[0] = cz;  mz.m[1] = -sz;
    mz.m[4] = sz;  mz.m[5] = cz;

    // For brevity, just return identity (stub)
    // TODO: Implement full mat4 multiply for real use
    return mat4_identity();
}

mat4 mat4_frustum_asymmetric(float l, float r, float b, float t, float n, float f) {
    mat4 m = {0};
    m.m[0] = (2.0f * n) / (r - l);
    m.m[5] = (2.0f * n) / (t - b);
    m.m[8] = (r + l) / (r - l);
    m.m[9] = (t + b) / (t - b);
    m.m[10] = -(f + n) / (f - n);
    m.m[11] = -1.0f;
    m.m[14] = -(2.0f * f * n) / (f - n);
    return m;
}

// --- matrix-vector ops ---

vec3 mat4_mul_vec3(mat4 m, vec3 v) {
    float w = m.m[3]*v.x + m.m[7]*v.y + m.m[11]*v.z + m.m[15];
    vec3 out;
    out.x = (m.m[0]*v.x + m.m[4]*v.y + m.m[8]*v.z + m.m[12]) / w;
    out.y = (m.m[1]*v.x + m.m[5]*v.y + m.m[9]*v.z + m.m[13]) / w;
    out.z = (m.m[2]*v.x + m.m[6]*v.y + m.m[10]*v.z + m.m[14]) / w;
    return out;
}

// --- visualization ---

void create_test_pgm(const char *filename, vec3 *points, int point_count, int width, int height) {
    unsigned char *data = (unsigned char*)calloc(width * height, 1);
    if (!data) return;

    for (int i = 0; i < point_count; i++) {
        int x = (int)((points[i].x + 1.0f) * 0.5f * (width - 1));
        int y = (int)((points[i].y + 1.0f) * 0.5f * (height - 1));
        if (x >= 0 && x < width && y >= 0 && y < height) {
            data[y * width + x] = 255;
        }
    }

    FILE *f = fopen(filename, "wb");
    if (f) {
        fprintf(f, "P5\n%d %d\n255\n", width, height);
        fwrite(data, 1, width * height, f);
        fclose(f);
    }

    free(data);
}
