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