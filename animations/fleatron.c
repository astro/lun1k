#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "main.h"
#include "libs/armmath.h"

#define MODE_DEAD 0
#define MODE_ALIVE 1
#define MODE_ZOMBIE 2

#define DIR_UP 0
#define DIR_LEFT 1
#define DIR_DOWN 2
#define DIR_RIGHT 3

struct racer {
    int mode;
    int x, y;
    int dir;
    uint8_t rgb[3];
};


#define N_RACERS 16
static struct racer *racers;

static void init(void) {
    racers = malloc(N_RACERS * sizeof(*racers));
    for(int i = 0; i < N_RACERS; i++)
        racers[i].mode = MODE_DEAD;

    lcdFillRGB(0, 0, 0);
}

static void deinit(void) {
    free(racers);
}

static int try_go(struct racer *r, int dir) {
    int dx = 0, dy = 0;
            
    switch(dir) {
    case DIR_UP:
        dy = -1;
        break;
    case DIR_LEFT:
        dx = 1;
        break;
    case DIR_DOWN:
        dy = 1;
        break;
    case DIR_RIGHT:
        dx = -1;
        break;
    }

    int nx = r->x + dx, ny = r->y + dy;
    if (nx < 0 || nx >= LED_WIDTH || ny < 0 || ny >= LED_HEIGHT)
        return 0;

    uint8_t rgb[3];
    getLedXY(nx, ny, rgb, rgb + 1, rgb + 2);

    int success = 0;
    switch(r->mode) {
    case MODE_ALIVE:
        success = rgb[0] < 128;
        break;
    case MODE_ZOMBIE:
        success = rgb[0] > 127;
        break;
    }

    if (success) {
        r->x = nx;
        r->y = ny;
        r->dir = dir;
    }
    return success;
}

static uint8_t tick(void) {
    for(int i = 0; i < N_RACERS; i++) {
        struct racer *r = racers + i;
        /* printf("racer %i: %ix%i MODE=%i\n", i, r->x, r->y, r->mode); */
        if (r->mode == MODE_DEAD) {
            /* Spawn a new */
            r->mode = MODE_ALIVE;
            r->x = rand() % LED_WIDTH;
            r->y = rand() % LED_HEIGHT;
            r->dir = rand() % 4;
            r->rgb[0] = 0x7f + (rand() & 0x7f);
            r->rgb[1] = 0x7f + (rand() & 0x7f);
            r->rgb[2] = 0x7f + (rand() & 0x7f);
            break;
        } else {
            if (r->mode == MODE_ALIVE)
                setLedXY(r->x, r->y, r->rgb[0], r->rgb[1], r->rgb[2]);
            else if (r->mode == MODE_ZOMBIE)
                /* setLedXY(r->x, r->y, 255 - r->rgb[0], 255 - r->rgb[1], 255 - r->rgb[2]); */
                setLedXY(r->x, r->y, 0, 0, 0);

            int should_turn = r->mode == MODE_ALIVE && (rand() & 0xF) == 0;
            if (!should_turn && try_go(r, r->dir)) {
                /* Went further in direction */
            } else {
                int new_dir;
                if (r->dir == DIR_UP || r->dir == DIR_DOWN) {
                    if ((rand() & 1) == 0)
                        new_dir = DIR_LEFT;
                    else
                        new_dir = DIR_RIGHT;
                } else {
                    if ((rand() & 1) == 0)
                        new_dir = DIR_UP;
                    else
                        new_dir = DIR_DOWN;
                }
                if (try_go(r, new_dir)) {
                    /* Ok */
                } else {
                    /* Give other direction a try */
                    new_dir = (new_dir + 2) % 4;
                    if (try_go(r, new_dir)) {
                        /* Ok */
                    } else {
                        /* Die */
                        r->mode = (r->mode + 1) % 3;
                        /* Reverse */
                        r->dir = (r->dir + 2) % 4;
                    }
                }
            }
        }
    }

    return 0;
}

static void constructor(void) CONSTRUCTOR_ATTRIBUTES
    void constructor(void) {
        registerAnimation("Flea Tron", init,tick,deinit, 10, 2000);
}
