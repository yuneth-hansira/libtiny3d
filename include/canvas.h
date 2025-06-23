// canvas.h
#ifndef CANVAS_H
#define CANVAS_H

typedef struct {
    int width;
    int height;
    float **pixels; // 2D array: brightness between 0.0 and 1.0
} canvas_t;

canvas_t *create_canvas(int width, int height);
void destroy_canvas(canvas_t *canvas);
void set_pixel_f(canvas_t *canvas, float x, float y, float intensity);
void draw_line_f(canvas_t *canvas, float x0, float y0, float x1, float y1, float thickness);
void save_canvas_as_pgm(canvas_t *canvas, const char *filename);

#endif