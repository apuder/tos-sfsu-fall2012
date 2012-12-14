
#include <kernel.h>

// WINDOW_ID fireworks_win_id;

int next_color = 0;

#define SPARK_COUNT       16
#define BOMB_COUNT        8
#define WINDOW_WIDTH      (8 * 32)
#define WINDOW_HEIGHT     180 // (8 * 12)

#define G 9.81f
#define T 0.1f
#define dV (T * G)

WINDOW fireworks_win = {41, 21, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, ' '};

typedef struct {
    float x;
    float y;
    int pixel_x;
    int pixel_y;
    int last_pixel_x;
    int last_pixel_y;
    float vx;
    float vy;
} SPARK;

typedef struct {
    SPARK sparks[SPARK_COUNT];
} BOMB;

static BOMB bombs[BOMB_COUNT];


// Pseudo random number generator
// http://cnx.org/content/m13572/latest/
int fw_seed = 17489;
int fw_last_random_number = 0;

int fw_random() {
    fw_last_random_number = (25173 * fw_last_random_number + 13849) % 65536;
    return fw_last_random_number;
}

void init_bomb(BOMB* b) {
    int i;

    int x = (int) (fw_random() % WINDOW_WIDTH);
    int y = (int) (fw_random() % WINDOW_HEIGHT);

    for (i = 0; i < SPARK_COUNT; i++) {
        b->sparks[i].x = x;
        b->sparks[i].y = y;
        b->sparks[i].vx = (float) ((fw_random() % 60) - 30);
        b->sparks[i].vy = (float) ((fw_random() % 60) - 30);
    }
}

void draw_sparc(int x, int y, tos_color color) {
    if (x < 0 || x >= WINDOW_WIDTH) return;
    if (y < 0 || y >= WINDOW_HEIGHT) return;
    // win_draw_pixel(fireworks_win_id, x, y, color);
    win_draw_pixel(&fireworks_win, x, y, color);
}

void calc_next_step(BOMB* bomb, float secondsPassed) {
    int i;

    for (i = 0; i < SPARK_COUNT; ++i) {
        // draw_sparc(bomb->sparks[i].last_pixel_x, bomb->sparks[i].last_pixel_y, COLOR_BLACK);
        draw_sparc(bomb->sparks[i].last_pixel_x, bomb->sparks[i].last_pixel_y, COLOR_BLACK);
        // Calculate new positions
        bomb->sparks[i].vy += dV;
        bomb->sparks[i].y += secondsPassed * bomb->sparks[i].vy;
        bomb->sparks[i].x += secondsPassed * bomb->sparks[i].vx;

        bomb->sparks[i].last_pixel_x = bomb->sparks[i].pixel_x;
        bomb->sparks[i].last_pixel_y = bomb->sparks[i].pixel_y;
        bomb->sparks[i].pixel_x = (int) bomb->sparks[i].x;
        bomb->sparks[i].pixel_y = (int) bomb->sparks[i].y;

        // Update the position
        draw_sparc(bomb->sparks[i].pixel_x, bomb->sparks[i].pixel_y, next_color);
        next_color = (next_color + 1) % 16;
    }
}

void win_fireworks(PROCESS self, PARAM param) {
    WORD win_x = 8 * 37;
    WORD win_y = 8 * 48;
    WORD win_width = 8 * 32;
    WORD win_height = 8 * 10;
    int counter = 0;
    int pointer = 0;
    int i;

    // Create window.
    // fireworks_win_id = win_spawn(win_x, win_y, win_width, win_height, "TOS Fireworks", COLOR_BLACK);

    // Init bombs
    for (i = 0; i < BOMB_COUNT; i++)
        init_bomb(&bombs[i]);

    while (42) {
        // eventually should have something fancier here		
        sleep(1);
        if (counter-- <= 0) {
            counter = 15;
            pointer = (pointer + 1) % BOMB_COUNT;
            init_bomb(&bombs[pointer]);
        }

        for (i = 0; i < BOMB_COUNT; ++i) {
            calc_next_step(&bombs[i], T);
        }
    }
}

void start_win_fireworks() {
    create_process(win_fireworks, 2, 0, "TOS Fireworks");
    resign();
}

