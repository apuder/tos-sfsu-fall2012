#include <kernel.h>
#include <vga.h>
#include <vga_draw.h>
#include <math.h>

void set_rect(rectangle* rect, int left, int top, int width, int height) {
    rect->left = left;
    rect->top = top;
    rect->width = width;
    rect->height = height;
}

BOOL within_bounds(int x, int y, rectangle* bounds) {
    return (x >= bounds->left && x < bounds->left + bounds->width) &&
            (y >= bounds->top && y < bounds->top + bounds->height);
}

BOOL masked(int x, int y, Mask mask) {
    if (mask == NULL) {
        return FALSE;
    }
    int i;
    for (i = 0; i < mask->num_mask; i++) {
        if ((x >= mask->masks[i].left && x < mask->masks[i].left + mask->masks[i].width) &&
                (y >= mask->masks[i].top && y < mask->masks[i].top + mask->masks[i].height)) {
            return TRUE;
        }
    }
    return FALSE;
}

void tos_fill_segment(unsigned start, unsigned length, tos_color color,
        rectangle* bounds) {
    int x, y, bmark, i = start;
    if (bounds == NULL) {
        draw_fill_segment(start, length, color);
        return;
    }
    while (i < start + length) {
        bmark = i;
        do {
            x = i % get_width();
            y = i / get_width();
            i++;
        } while (i <= start + length && within_bounds(x, y, bounds));
        if (bmark != i + 1) {
            draw_fill_segment(bmark, (i - 1) - bmark, color);
        }
    }
}

void tos_fill_segment_masked(unsigned start, unsigned length, tos_color color,
        rectangle* bounds, Mask mask) {
    int x, y, bmark, i = start;
    while (i < start + length) {
        bmark = i;
        do {
            x = i % get_width();
            y = i / get_width();
            i++;
        } while (i <= start + length &&
                (bounds == NULL || within_bounds(x, y, bounds)) && !masked(x, y, mask));
        if (bmark != i + 1) {
            draw_fill_segment(bmark, (i - 1) - bmark, color);
        }
    }
}

void tos_write_char(char c, int x, int y, tos_color color,
        rectangle* bounds) {
    if (within_bounds(x, y, bounds)) {
        draw_char(c, x, y, color);
    }
}

void tos_write_char_masked(char c, int x, int y, tos_color color,
        rectangle* bounds, Mask mask) {
    if (within_bounds(x, y, bounds) && !masked(x, y, mask)) {
        draw_char(c, x, y, color);
    }
}

/****** WINDOWING SYSTEMS LOGICAL LAYER ******/
//NOTE: These return ints for a way to identify the object

/**
 * Draws a line to the screen using the current color
 */
int draw_line(tos_color c, tos_color stroke, unsigned int style,
        int x1, int y1, int x2, int y2, rectangle* bounds) {

    //	int screen_w = get_width();

    // Rasterizes a line using Bresenhams algo
    // Set starting and ending x y values so that x0 < xEnd
    int x0, y0, xEnd, yEnd;
    int k; //used for stroke
    if (x1 < x2) {
        // Start from v1
        x0 = x1;
        y0 = y1;
        xEnd = x2;
        yEnd = y2;
    } else {
        // Start from v2
        x0 = x2;
        y0 = y2;
        xEnd = x1;
        yEnd = y1;
    }

    int dx = xEnd - x0;
    int dy = yEnd - y0;
    int x = x0, y = y0;

    // Special cases
    if (dx == 0) {
        if (y0 < yEnd) {
            for (y = y0; y < yEnd; y++) {
                for (k = x; k < x + stroke; k++) {
                    y = y - 1;
                    if (within_bounds(k, y, bounds)) {
                        tos_graphics.write_pixel(k, y, c);
                    }
                }
            }
        } else {
            for (y = y0; y > yEnd; y--) {
                for (k = x; k < x + stroke; k++) {
                    y = y - 1;
                    if (within_bounds(k, y, bounds)) {
                        tos_graphics.write_pixel(k, y, c);
                    }
                }
            }
        }
        return 0;
    }

    if (dy == 0) {
        for (x = x0; x < xEnd; x++) {
            for (k = y; k < y + stroke; k++) {
                if (within_bounds(x, k, bounds)) {
                    tos_graphics.write_pixel(x, k, c);
                }
            }
        }
        return 0;
    }

    int p = 0, twoDy = 0, twoDyMinusDx = 0, step = 0;
    BOOL stepX = FALSE, stepY = FALSE;
    if (dx == dy) {
        p = 2 * dy - dx;
        twoDy = 2 * dy;
        twoDyMinusDx = 2 * (dy - dx);
        step = 1;
        stepX = TRUE;
    }

    if (dx == -dy) {
        dy = -dy;
        p = 2 * dy - dx;
        twoDy = 2 * dy;
        twoDyMinusDx = 2 * (dy - dx);
        step = -1;
        stepX = TRUE;
    }

    // General cases
    float m = dy / (float) dx;
    if (m > 0.0 && m < 1.0) {
        p = 2 * dy - dx;
        twoDy = 2 * dy;
        twoDyMinusDx = 2 * (dy - dx);
        step = 1;
        stepX = TRUE;
    }

    if (m > -1.0 && m < 0.0) {
        dy = -dy;
        p = 2 * dy - dx;
        twoDy = 2 * dy;
        twoDyMinusDx = 2 * (dy - dx);
        step = -1;
        stepX = TRUE;
    }

    if (m > 1.0) {
        p = 2 * dx - dy;
        twoDy = 2 * dx;
        twoDyMinusDx = 2 * (dx - dy);
        step = 1;
        stepY = TRUE;
    }

    if (m < -1.0) {
        dx = -dx;
        p = 2 * dx - dy;
        twoDy = -2 * dx;
        twoDyMinusDx = -2 * (dx - dy);
        // Start from v2 
        int tmp = x0;
        x0 = xEnd;
        xEnd = tmp;
        tmp = y0;
        y0 = yEnd;
        yEnd = tmp;
        step = -1;
        stepY = TRUE;
    }

    x = x0;
    y = y0;

    // Actual line drawing
    // Done by either stepping through x or y axis
    if (stepX) {
        // The first point
        for (k = y; k < y + stroke; k++) {
            if (within_bounds(x, k, bounds)) {
                tos_graphics.write_pixel(x, k, c);
            }
        }
        // Step through x
        while (x < xEnd) {
            x++;
            if (p < 0)
                p += twoDy;
            else {
                y += step;
                p += twoDyMinusDx;
            }
            for (k = y; k < y + stroke; k++) {
                if (within_bounds(x, k, bounds)) {
                    tos_graphics.write_pixel(x, k, c);
                }
            }
        }
    }

    if (stepY) {
        // The first point
        for (k = x; k < x + stroke; k++) {
            if (within_bounds(k, y, bounds)) {
                tos_graphics.write_pixel(k, y, c);
            }
        }

        // Step through y
        while (y < yEnd) {
            y++;
            if (p < 0)
                p += twoDy;
            else {
                x += step;
                p += twoDyMinusDx;
            }
            for (k = x; k < x + stroke; k++) {
                if (within_bounds(k, y, bounds)) {
                    tos_graphics.write_pixel(k, y, c);
                }
            }
        }
    }

    return 0;

}

int draw_rect(tos_color fill, tos_color outline, unsigned int stroke,
        int left, int top, unsigned int width, unsigned int height, rectangle* bounds) {
    int rows;
    //fix_bounds(&left, &top, &width, &height, bounds);
    int screen_w = get_width();
    if (outline != NONE) {
        for (rows = top; rows < top + stroke; rows++) {
            tos_fill_segment(left + screen_w * (rows), width, outline, bounds);
        }
        for (rows = top + stroke; rows < top + height - stroke; rows++) {
            tos_fill_segment(left + screen_w * (rows), stroke, outline, bounds);
            if (fill != NONE) {
                tos_fill_segment(left + screen_w * (rows) + stroke, width - stroke, fill, bounds);
            }
            tos_fill_segment(left + screen_w * (rows) + (width - stroke), stroke, outline, bounds);
        }
        for (rows = top + height - stroke; rows < top + height; rows++) {
            tos_fill_segment(left + screen_w * (rows), width, outline, bounds);
        }
    } else {
        for (rows = top; rows < top + height; rows++) {
            tos_fill_segment(left + screen_w * (rows), width, fill, bounds);
        }
    }
    return 0;
}

int draw_rect_masked(tos_color fill, tos_color outline, unsigned int stroke,
        int left, int top, unsigned int width, unsigned int height,
        rectangle* bounds, Mask mask) {
    char msg[80];
    // sprintf(msg, "mask->num_mask %d", mask->num_mask);
    draw_string(RED, 0, 200, msg);
    int rows;
    //fix_bounds(&left, &top, &width, &height, bounds);
    int screen_w = get_width();
    if (outline != NONE) {
        for (rows = top; rows < top + stroke; rows++) {
            tos_fill_segment_masked(left + screen_w * (rows), width, outline, bounds, mask);
        }
        for (rows = top + stroke; rows < top + height - stroke; rows++) {
            tos_fill_segment_masked(left + screen_w * (rows), stroke, outline, bounds, mask);
            if (fill != NONE) {
                tos_fill_segment_masked(left + screen_w * (rows) + stroke, width - stroke, fill, bounds, mask);
            }
            tos_fill_segment_masked(left + screen_w * (rows) + (width - stroke), stroke, outline, bounds, mask);
        }
        for (rows = top + height - stroke; rows < top + height; rows++) {
            tos_fill_segment_masked(left + screen_w * (rows), width, outline, bounds, mask);
        }
    } else {
        for (rows = top; rows < top + height; rows++) {
            tos_fill_segment_masked(left + screen_w * (rows), width, fill, bounds, mask);
        }
    }
    return 0;
}

int draw_circle(tos_color fill, tos_color outline, unsigned int stroke, int centerx,
        int centery, unsigned int radius, rectangle* bounds) {
    //Equation: (x - h)^2 + (y - k)^2 = r^2
    unsigned int ymin = centery - radius;
    unsigned int ymax = centery + radius;
    unsigned int xmin = centerx - radius;
    unsigned int xmax = centerx + radius;
    return draw_ellipse(fill, outline, stroke,
            xmin, ymin, xmax - xmin, ymax - ymin, bounds);
}

int draw_circle_masked(tos_color fill, tos_color outline, unsigned int stroke, int centerx,
        int centery, unsigned int radius, rectangle* bounds, Mask mask) {
    //Equation: (x - h)^2 + (y - k)^2 = r^2
    unsigned int ymin = centery - radius;
    unsigned int ymax = centery + radius;
    unsigned int xmin = centerx - radius;
    unsigned int xmax = centerx + radius;
    return draw_ellipse_masked(fill, outline, stroke,
            xmin, ymin, xmax - xmin, ymax - ymin, bounds, mask);
}

int draw_ellipse(tos_color fill, tos_color outline, unsigned int stroke,
        int left, int top, unsigned int width, unsigned int height, rectangle* bounds) {
    //Equation: (x - h)^2 / a^2  + (y - k)^2 / b^2 = 1
    float ymin = top;
    float ymax = top + height;
    float stk_ymin = ymin - stroke;
    float stk_ymax = ymax + stroke;
    float xmin = left;
    float xmax = left + width;
    float stk_xmin = xmin - stroke;
    float stk_xmax = xmax + stroke;
    float a = (float) (xmax - xmin) / 2.0f;
    float b = (float) (ymax - ymin) / 2.0f;
    float stk_a = (float) (stk_xmax - stk_xmin) / 2.0f;
    float stk_b = (float) (stk_ymax - stk_ymin) / 2.0f;
    float centerx = (float) xmin + a;
    float centery = (float) ymin + b;
    float stk_centerx = (float) stk_xmin + stk_a;
    float stk_centery = (float) stk_ymin + stk_b;

    unsigned int j;
    if (outline != NONE) {
        for (j = stk_ymin; j <= stk_ymax; j++) {
            stk_xmin = stk_centerx - sqrtf(power(stk_a, 2) * (1.0f - power((float) j - stk_centery, 2) / power(stk_b, 2)));
            stk_xmax = stk_centerx + sqrtf(power(stk_a, 2) * (1.0f - power((float) j - stk_centery, 2) / power(stk_b, 2)));
            if (j > ymin && j <= ymax) {
                xmin = centerx - sqrtf(power(a, 2) * (1.0f - power((float) j - centery, 2) / power(b, 2)));
                xmax = centerx + sqrtf(power(a, 2) * (1.0f - power((float) j - centery, 2) / power(b, 2)));
                //fix_bounds(&xmin, j, &xmax - &xmin, 0, bounds); 
                if (fill != NONE)
                    tos_fill_segment(xmin + tos_graphics.width * j, xmax - xmin, fill, bounds);
                tos_fill_segment(stk_xmin + tos_graphics.width * j, xmin - stk_xmin, outline, bounds);
                tos_fill_segment(xmax + tos_graphics.width * j, stk_xmax - xmax, outline, bounds);
            } else {
                tos_fill_segment(stk_xmin + tos_graphics.width * j, stk_xmax - stk_xmin, outline, bounds);
            }
        }
    } else {
        if (fill != NONE) {
            for (j = ymin; j <= ymax; j++) {
                xmin = centerx - sqrtf(power(a, 2) * (1.0f - power((float) j - centery, 2) / power(b, 2)));
                xmax = centerx + sqrtf(power(a, 2) * (1.0f - power((float) j - centery, 2) / power(b, 2)));
                tos_fill_segment(xmin + tos_graphics.width * j, xmax - xmin, fill, bounds);
            }
        }
    }

    return 0;
}

int draw_ellipse_masked(tos_color fill, tos_color outline, unsigned int stroke,
        int left, int top, unsigned int width, unsigned int height,
        rectangle* bounds, Mask mask) {
    //Equation: (x - h)^2 / a^2  + (y - k)^2 / b^2 = 1
    float ymin = top;
    float ymax = top + height;
    float stk_ymin = ymin - stroke;
    float stk_ymax = ymax + stroke;
    float xmin = left;
    float xmax = left + width;
    float stk_xmin = xmin - stroke;
    float stk_xmax = xmax + stroke;
    float a = (float) (xmax - xmin) / 2.0f;
    float b = (float) (ymax - ymin) / 2.0f;
    float stk_a = (float) (stk_xmax - stk_xmin) / 2.0f;
    float stk_b = (float) (stk_ymax - stk_ymin) / 2.0f;
    float centerx = (float) xmin + a;
    float centery = (float) ymin + b;
    float stk_centerx = (float) stk_xmin + stk_a;
    float stk_centery = (float) stk_ymin + stk_b;

    unsigned int j;
    if (outline != NONE) {
        for (j = stk_ymin; j <= stk_ymax; j++) {
            stk_xmin = stk_centerx -
                    sqrtf(power(stk_a, 2) * (1.0f - power((float) j - stk_centery, 2) /
                    power(stk_b, 2)));
            stk_xmax = stk_centerx +
                    sqrtf(power(stk_a, 2) * (1.0f - power((float) j - stk_centery, 2) /
                    power(stk_b, 2)));
            if (j > ymin && j <= ymax) {
                xmin = centerx -
                        sqrtf(power(a, 2) * (1.0f - power((float) j - centery, 2) /
                        power(b, 2)));
                xmax = centerx +
                        sqrtf(power(a, 2) * (1.0f - power((float) j - centery, 2) /
                        power(b, 2)));
                //fix_bounds(&xmin, j, &xmax - &xmin, 0, bounds); 
                if (fill != NONE) {
                    tos_fill_segment_masked(xmin + tos_graphics.width * j,
                            xmax - xmin, fill, bounds, mask);
                }
                tos_fill_segment_masked(stk_xmin + tos_graphics.width * j,
                        xmin - stk_xmin, outline, bounds, mask);
                tos_fill_segment_masked(xmax + tos_graphics.width * j,
                        stk_xmax - xmax, outline, bounds, mask);
            } else {
                tos_fill_segment_masked(stk_xmin + tos_graphics.width * j,
                        stk_xmax - stk_xmin, outline, bounds, mask);
            }
        }
    } else {
        if (fill != NONE) {
            for (j = ymin; j <= ymax; j++) {
                xmin = centerx -
                        sqrtf(power(a, 2) * (1.0f - power((float) j - centery, 2) /
                        power(b, 2)));
                xmax = centerx +
                        sqrtf(power(a, 2) * (1.0f - power((float) j - centery, 2) /
                        power(b, 2)));
                tos_fill_segment_masked(xmin + tos_graphics.width * j,
                        xmax - xmin, fill, bounds, mask);
            }
        }
    }

    return 0;
}

int draw_arc(tos_color c, unsigned int stroke, int centerx, int centery, unsigned int major_width,
        unsigned int minor_width, int angle1, int angle2, rectangle* bounds) {

    float rad;
    int xi, yi;
    float i, j;
    for (j = 0; j < stroke; j++) {
        for (i = angle1; i < angle2; i += 0.1) {
            rad = (i * PI) / 180.0f;
            xi = (int) (cosf(rad) * (float) major_width) + centerx;
            yi = (int) (sinf(rad) * (float) minor_width) + centery;
            if (within_bounds(xi, yi, bounds)) {
                tos_graphics.write_pixel(xi, yi, c);
            }
        }
        major_width++;
        minor_width++;
    }
    return 0;
}

void draw_string(tos_color c, unsigned int x, unsigned int y, unsigned char* chars) {
    while (*chars != 0) {
        if (x + 8 <= tos_graphics.width) {
            tos_graphics.write_char(*(chars++), x, y, c);
            x += 8;
        } else {
            y += 8;
            x = x % 8;
            tos_graphics.write_char(*(chars++), x, y, c);
            x += 8;
        }
    }
}

void draw_string_masked(tos_color c, unsigned int x, unsigned int y,
        unsigned char* chars, rectangle* bounds, Mask mask) {
    while (*chars != 0) {
        if (x + 8 <= tos_graphics.width) {
            tos_write_char_masked(*(chars++), x, y, c, bounds, mask);
            x += 8;
        } else {
            y += 8;
            x = x % 8;
            tos_write_char_masked(*(chars++), x, y, c, bounds, mask);
            x += 8;
        }
    }
}

void demo_graphics() {
    set_clear_color(WHITE);

    clear_graphics();
    unsigned colors[960];

    int m;
    for (m = 0; m < 960; m++) {
        colors[m] = m % 16;
    }

    read_scan_segment(0, 960, colors);

    set_clear_color(BLUE);

    clear_graphics();

    draw_scan_segment(0, 960, colors);

    int i, j, c;
    for (i = 0, c = 0; i < get_width(); i += 8) {
        draw_string(c++ % 16, i, i, "TOS Graphics ROCK!!!");
    }

    for (i = 1; i <= 8; i++) {
        // draw_rect(RED, NONE, 1, i, i * 4, 17, 4);
    }

    // draw_ellipse(NONE, RED, 1, 100, 100, 80, 100);
    rectangle* bounds = NULL;
    rectangle* bounds2 = NULL;
    //set_rect(bounds, 300, 300, 100, 100);
    //set_rect(bounds, 300, 300, 50, 50);
    set_rect(bounds, 200, 200, 50, 50);
    set_rect(bounds2, 0, 0, 640, 480);
    draw_line(YELLOW, 10, 0, 200, 200, 300, 300, bounds);
    draw_line(RED, 10, 0, 0, 100, 302, 302, bounds2);
    //draw_arc(BLACK, 1, 200, 200, 50, 50, 0, 380, bounds);
    //draw_rect(BLUE, BLACK, 1, 200, 200, 100, 100, bounds);
    //draw_circle(BLUE, BLACK, 1, 250, 250, 50, bounds);
    while (1);
}

