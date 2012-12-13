/****** WINDOWING SYSTEMS LOGICAL LAYER ******/
//NOTE: These return ints for a way to identify the object

#include <vga.h>

#ifndef __TOS_DRAW__
#define __TOS_DRAW__

typedef struct {
    int left;
    int top;
    int width;
    int height;
} rectangle;

typedef struct _Mask* Mask;

typedef struct _Mask {
    rectangle* masks;
    int num_mask;
} Mask_struct;


void set_rect(rectangle* rect, int left, int top, int width, int height);
int within_bounds(int x, int y, rectangle* bounds);
void tos_fill_segment(unsigned start, unsigned length, tos_color color, rectangle* bounds);

/**
 * Draws a line to the screen
 * c - color of the line
 * stroke - thickness of the line
 * style - dashed, dot-dashed, etc.  **UNIMPLEMENTED**
 * x1 - start of line - x-coordinate
 * y1 - start of line - y-coordinate
 * x2 - end of line - x-coordinate
 * y2 - end of line - y-coordinate
 */
int draw_line(tos_color c, tos_color stroke, unsigned int style,
        int x1, int y1, int x2, int y2, rectangle* bounds);


/**
 * Draws a rectangle on the screen
 * fill - color of the inside of the rectangle
 * outline - color of the outside frame of the rectangle
 * stroke - thickness of the outline
 * left - top-left coordinate of rectangle - x-coordinate
 * top - top-left coordinate of rectangle - y-coordinate
 * width - the width from top-left - x-coordinate
 * height - the height from top-left - y-coordinate
 */
int draw_rect(tos_color fill, tos_color outline, unsigned int stroke,
        int left, int top, unsigned int width, unsigned int height, rectangle* bounds);

int draw_rect_masked(tos_color fill, tos_color outline, unsigned int stroke,
        int left, int top, unsigned int width, unsigned int height,
        rectangle* bounds, Mask mask);

/**
 * Draws a circle on the screen
 * NOTE: calls draw_ellipse to draw
 * fill - color of the inside of the circle
 * outline - color of the outside frame of the circle
 * stroke - thickness of the outline
 * centerx - center of the circle - x-coordinate
 * centery - center of the circle - y-coordinate
 * radius - the radius of the circle
 */
int draw_circle(tos_color fill, tos_color outline, unsigned int stroke,
        int centerx, int centery, unsigned int radius, rectangle* bounds);

int draw_circle_masked(tos_color fill, tos_color outline, unsigned int stroke,
        int centerx, int centery, unsigned int radius,
        rectangle* bounds, Mask mask);


/**
 * Draws an ellipse on the screen
 * fill - color of the inside of the ellipse
 * outline - color of the outside frame of the ellipse
 * stroke - thickness of the outline
 * left - top-left coordinate of ellipse - x-coordinate
 * top - top-left coordinate of ellipse - y-coordinate
 * width - the width from top-left - x-coordinate
 * height - the height from top-left - y-coordinate
 */
int draw_ellipse(tos_color fill, tos_color outline, unsigned int stroke,
        int left, int top, unsigned int width, unsigned int height, rectangle* bounds);

int draw_ellipse_masked(tos_color fill, tos_color outline, unsigned int stroke,
        int left, int top, unsigned int width, unsigned int height,
        rectangle* bounds, Mask mask);


/**
 * Draws an arc on the screen
 * c - color of the arc
 * stroke - thickness of the arc
 * left - top-left coordinate of arc - x-coordinate
 * top - top-left coordinate of arc - y-coordinate
 * width - the width from top-left - x-coordinate
 * height - the height from top-left - y-coordinate
 * angle1 - the start angle from where to start drawing
 * angle2 - the end angle where to stop drawing
 */
int draw_arc(tos_color c, unsigned int stroke, int centerx, int centery, unsigned int major_width,
        unsigned int major_height, int angle1, int angle2, rectangle* bounds);


/**
 * Draws a string on the screen
 * c - color of the string
 * x - location of start of string - x-coordinate
 * y - location of start of string - y-coordinate
 * chars - the array of characters to draw
 */
void draw_string(tos_color c, unsigned int x, unsigned int y, unsigned char* chars);

void draw_string_masked(tos_color c, unsigned int x, unsigned int y,
        unsigned char* chars, rectangle* bounds, Mask mask);


/**
 * Does a demo of the graphics functions
 */
void demo_graphics(void);

#endif
