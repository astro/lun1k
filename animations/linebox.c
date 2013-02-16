#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "main.h"
#include "libs/text.h"
#include "libs/armmath.h"
#include "libs/wuline.h"


/**
 * Helpers
 **/

#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

struct point {
    uint8_t x, y;
};
#define INTERVAL 16
struct point points[(LED_WIDTH + LED_HEIGHT) / INTERVAL];

void advance_point(struct point *p) {
    if (p->x < LED_WIDTH - 1 && p->y == 0)
        p->x++;
    else if (p->x == LED_WIDTH - 1 && p->y < LED_HEIGHT - 1)
        p->y++;
    else if (p->x > 0 && p->y == LED_HEIGHT - 1)
        p->x--;
    else if (p->x == 0 && p->y > 0)
        p->y--;
}

static void init(void) {
    struct point p = { 0, 0 };
    for(uint8_t i = 0; i < sizeof(points); i++) {
        points[i] = p;
        for(uint8_t j = 0; j < INTERVAL; j++)
            advance_point(&p);
    }
}

static void deinit(void) {
}

static uint8_t tick(void) {
    for(uint8_t i = 0; i < sizeof(points); i++) {
        advance_point(points + i);
    }

    for(uint8_t y = 0; y < LED_HEIGHT; y++)
        for(uint8_t x = 0; x < LED_WIDTH; x++)
            setLedXY(x, y, 0, 0, 0);
    for(uint8_t i = 0; i < sizeof(points); i++) {
        struct point *p1 = points + i;
        struct point *p2 = points + ((i + 13) % sizeof(points));
        draw_line(p1->x, p1->y, p2->x, p2->y, sini((uint16_t)getSysTick()*1.6f)>>8, sini(getSysTick()*2)>>8, sini(getSysTick())>>8);
    }

    return 0;
}

static void constructor(void) CONSTRUCTOR_ATTRIBUTES
    void constructor(void) {
        registerAnimation("Line Box", init,tick,deinit, 4, 1000);
}
