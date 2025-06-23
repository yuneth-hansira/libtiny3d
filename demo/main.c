// main.c
#include <stdio.h>
#include <math.h>
#include "canvas.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WIDTH 512
#define HEIGHT 512

int main() {
    canvas_t *canvas = create_canvas(WIDTH, HEIGHT);

    float center_x = WIDTH / 2.0f;
    float center_y = HEIGHT / 2.0f;
    float radius = 200.0f;

    for (int i = 0; i < 24; i++) {
        float angle_deg = i * 15.0f;
        float angle_rad = angle_deg * M_PI / 180.0f;

        float x = center_x + radius * cosf(angle_rad);
        float y = center_y + radius * sinf(angle_rad);

        draw_line_f(canvas, center_x, center_y, x, y, 2.0f);
    }

    save_canvas_as_pgm(canvas, "clock_lines.pgm");

    destroy_canvas(canvas);
    return 0;
}