/* Based on https://github.com/schultyy/-29--c-3-.logo-builder/blob/master/src/main.js */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "main.h"
#include "libs/armmath.h"


static char text[128];
static int ty;

static char wsSeps[] = ".-/";
static char *numDecorators[] = {
    "[]", "()"
};

static int8_t background[] = {
    0, 5, 0x32
};

static int8_t colors[6][3] = {
    { 0x56, 0xC5, 0xD0 },
    { 0xED, 0x1A, 0x3B },
    { 0x00, 0xB2, 0x6B },
    { 0xF7, 0x94, 0x33 },
    { 0x6D, 0x68, 0xAF },
    { 0xFF, 0xFF, 0xFF }
};

static int isDigit(char c) {
    return c >= '0' && c <= '9';
}

static int isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

void draw_char_8x6(int x,int y, char text, uint8_t r,uint8_t g, uint8_t b);

#ifndef SIMULATOR
extern uint8_t leds[LED_HEIGHT][LED_WIDTH][3] CCM_ATTRIBUTES;
static void fillLeds(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t *l = leds;
    for(int i = 0; i < LED_WIDTH * LED_HEIGHT; i++) {
        *(l++) = r;
        *(l++) = g;
        *(l++) = b;
    }
}
#else
static void fillLeds(uint8_t r, uint8_t g, uint8_t b) {
    lcdFillRGB(r, g, b);
}
#endif

static void init(void) {
    snprintf(text, sizeof(text), "NOT MY DEPARTMENT 29C3 ");
    ty = 0;
}

static void deinit(void) {
}

static uint8_t tick(void) {
    static uint32_t time = 0;
    time++;

    int flash = ((time >> 1) & 0x7) == 0;
    int8_t *color = colors[(time >> 4) % 6];

    if (flash)
        fillLeds(color[0], color[1], color[2]);
    else
        fillLeds(background[0], background[1], background[2]);

    char outputText[512];
    char *o = outputText;
    static int currentNumDecorator = -1;
    int previousWasDigit = 0;
    for(char *t = text; *t; t++) {
        if (isWhitespace(*t)) {
            *(o++) = wsSeps[(sizeof(wsSeps) - 1) * rand() / RAND_MAX];
        } else if (!previousWasDigit && isDigit(*t)) {
            currentNumDecorator = sizeof(numDecorators) / sizeof(*numDecorators) * rand() / RAND_MAX;
            *(o++) = numDecorators[currentNumDecorator][0];
            *(o++) = *t;
            previousWasDigit = 1;
        } else if (previousWasDigit && !isDigit(*t)) {
            *(o++) = *t;
            *(o++) = numDecorators[currentNumDecorator][1];
            previousWasDigit = 0;
            currentNumDecorator = -1;
        } else {
            *(o++) = *t;
        }
    }
    if (previousWasDigit)
        *(o++) = numDecorators[currentNumDecorator][1];
    *o = '\0';

    o = outputText;
    int8_t *rgb = flash ? background : color;
    for(int y = ty; y < LED_HEIGHT; y += 11) {
        for(int x = 1; x < LED_WIDTH; x += 8) {
            if (x == 1 && y == 0 && o == outputText)
                ty = 0;
            if (y > -12 && y < LED_HEIGHT)
                draw_char_8x6(x, y, *o, rgb[0], rgb[1], rgb[2]);
            if (!*(++o)) {
                o = outputText;
            }
        }
    }
    ty--;

    return 0;
}

static void constructor(void) CONSTRUCTOR_ATTRIBUTES
    void constructor(void) {
        registerAnimation("29C3", init,tick,deinit, 1000, 2000);
}
