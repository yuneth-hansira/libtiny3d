// canvas.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "canvas.h"

canvas_t *create_canvas(int width, int height) {
    canvas_t *canvas = (canvas_t *)malloc(sizeof(canvas_t));
    canvas->width = width;
    canvas->height = height;

    canvas->pixels = (float **)malloc(height * sizeof(float *));
    for (int y = 0; y < height; y++) {
        canvas->pixels[y] = (float *)calloc(width, sizeof(float));
    }

    return canvas;
}

void destroy_canvas(canvas_t *canvas) {
    for (int y = 0; y < canvas->height; y++) {
        free(canvas->pixels[y]);
    }
    free(canvas->pixels);
    free(canvas);
}

// Clamp values between 0 and 1
static float clamp(float x) {
    if (x < 0) return 0;
    if (x > 1) return 1;
    return x;
}

// Bilinear filtered pixel setting
void set_pixel_f(canvas_t *canvas, float x, float y, float intensity) {
    int x0 = (int)floorf(x);
    int y0 = (int)floorf(y);
    float dx = x - x0;
    float dy = y - y0;

    for (int dy_i = 0; dy_i <= 1; dy_i++) {
        for (int dx_i = 0; dx_i <= 1; dx_i++) {
            int xi = x0 + dx_i;
            int yi = y0 + dy_i;
            if (xi >= 0 && xi < canvas->width && yi >= 0 && yi < canvas->height) {
                float weight = (1 - fabsf(dx_i - dx)) * (1 - fabsf(dy_i - dy));
                canvas->pixels[yi][xi] += clamp(intensity * weight);
                canvas->pixels[yi][xi] = clamp(canvas->pixels[yi][xi]);
            }
        }
    }
}

// DDA algorithm for line drawing with thickness
void draw_line_f(canvas_t *canvas, float x0, float y0, float x1, float y1, float thickness) {
    float dx = x1 - x0;
    float dy = y1 - y0;
    float length = fmaxf(fabsf(dx), fabsf(dy));

    if (length == 0) return;

    dx /= length;
    dy /= length;

    for (int i = 0; i <= (int)length; i++) {
        float cx = x0 + dx * i;
        float cy = y0 + dy * i;

        // draw a circular thickness using distance formula
        for (float tx = -thickness / 2; tx <= thickness / 2; tx += 0.5f) {
            for (float ty = -thickness / 2; ty <= thickness / 2; ty += 0.5f) {
                if (tx * tx + ty * ty <= (thickness * thickness) / 4) {
                    set_pixel_f(canvas, cx + tx, cy + ty, 1.0f);
                }
            }
        }
    }
}

// Output to PGM for viewing
void save_canvas_as_pgm(canvas_t *canvas, const char *filename) {
    FILE *f = fopen(filename, "w");
    fprintf(f, "P2\n%d %d\n255\n", canvas->width, canvas->height);
    for (int y = 0; y < canvas->height; y++) {
        for (int x = 0; x < canvas->width; x++) {
            fprintf(f, "%d ", (int)(canvas->pixels[y][x] * 255));
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

// main.c
#include <stdio.h>
#include <math.h>
#include "canvas.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WIDTH 512
#define HEIGHT 512

// Simple 4x4 matrix and 3D vector for transform
typedef struct { float x, y, z; } vec3;
typedef struct { float m[16]; } mat4;

// Matrix functions (column-major)
mat4 mat4_identity() {
    mat4 m = {0};
    m.m[0] = m.m[5] = m.m[10] = m.m[15] = 1.0f;
    return m;
}
mat4 mat4_translate(float tx, float ty, float tz) {
    mat4 m = mat4_identity();
    m.m[12] = tx; m.m[13] = ty; m.m[14] = tz;
    return m;
}
mat4 mat4_scale(float sx, float sy, float sz) {
    mat4 m = mat4_identity();
    m.m[0] = sx; m.m[5] = sy; m.m[10] = sz;
    return m;
}
mat4 mat4_frustum(float l, float r, float b, float t, float n, float f) {
    mat4 m = {0};
    m.m[0] = 2*n/(r-l);
    m.m[5] = 2*n/(t-b);
    m.m[8] = (r+l)/(r-l);
    m.m[9] = (t+b)/(t-b);
    m.m[10] = -(f+n)/(f-n);
    m.m[11] = -1;
    m.m[14] = -2*f*n/(f-n);
    return m;
}
vec3 mat4_mul_vec3(mat4 m, vec3 v) {
    float x = v.x, y = v.y, z = v.z;
    float w = m.m[3]*x + m.m[7]*y + m.m[11]*z + m.m[15];
    vec3 out;
    out.x = (m.m[0]*x + m.m[4]*y + m.m[8]*z + m.m[12]) / w;
    out.y = (m.m[1]*x + m.m[5]*y + m.m[9]*z + m.m[13]) / w;
    out.z = (m.m[2]*x + m.m[6]*y + m.m[10]*z + m.m[14]) / w;
    return out;
}

int main() {
    // Cube vertices
    vec3 cube[8] = {
        {-1,-1,-1}, {1,-1,-1}, {1,1,-1}, {-1,1,-1},
        {-1,-1,1},  {1,-1,1},  {1,1,1},  {-1,1,1}
    };
    // Cube edges
    const int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };

    // Transform: scale, translate, project
    mat4 s = mat4_scale(0.5f, 0.5f, 0.5f);
    mat4 t = mat4_translate(0, 0, 4.0f);
    mat4 p = mat4_frustum(-1,1,-1,1,1,10);

    // Projected 2D points
    float proj_x[8], proj_y[8];
    for (int i = 0; i < 8; i++) {
        vec3 v = cube[i];
        v = mat4_mul_vec3(s, v);
        v = mat4_mul_vec3(t, v);
        v = mat4_mul_vec3(p, v);
        // Map from [-1,1] to image coordinates
        proj_x[i] = (v.x + 1.0f) * 0.5f * (WIDTH-1);
        proj_y[i] = (v.y + 1.0f) * 0.5f * (HEIGHT-1);
    }

    canvas_t *canvas = create_canvas(WIDTH, HEIGHT);

    // Draw all edges
    for (int i = 0; i < 12; i++) {
        int a = edges[i][0], b = edges[i][1];
        draw_line_f(canvas, proj_x[a], proj_y[a], proj_x[b], proj_y[b], 2.0f);
    }

    save_canvas_as_pgm(canvas, "cube_canvas.pgm");
    destroy_canvas(canvas);
    printf("PGM wireframe image generated: cube_canvas.pgm\n");
    return 0;
}