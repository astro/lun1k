#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "main.h"
#include "libs/armmath.h"
#include "libs/zint/zint.h"
#include "libs/zint/common.h"

#define BORDER 3
int qr_code(struct zint_symbol *symbol, unsigned char source[], int length);

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))


static void init(void) {
    lcdFillRGB(255, 255, 255);
}

static void deinit(void) {
}

static uint8_t tick(void) {
    float time = getSysTick() / 10000.0f;
    char text[128];
    snprintf(text, 127, "Device Time: %.1fs", time);

    struct zint_symbol zs;
    memset(&zs, 0, sizeof(zs));
    zs.input_mode = DATA_MODE;
    zs.option_1 = 3;
    /* zs.option_2 = 2; */
    int qr_res = qr_code(&zs, (unsigned char *)text, strlen(text));
    if (qr_res) {
        /* printf("Error: %i\n", qr_res); */
    }

    int zoom = MAX(1, MIN(LED_WIDTH / (zs.width + (BORDER * 2)), LED_HEIGHT / (zs.rows + (BORDER * 2))));
    static int old_width = 0;
    if (old_width != zs.width) {
        old_width = zs.width;
        lcdFillRGB(255, 255, 255);
    }

    int x0 = (LED_WIDTH - zoom * zs.width) / 2;
    int y0 = (LED_HEIGHT - zoom * zs.rows) / 2;
    for(int y = 0; y < zs.width; y++) {
        for(int x = 0; x < zs.rows; x++) {
            for(int y1 = y * zoom; y1 < (y + 1) * zoom; y1++) {
                for(int x1 = x * zoom; x1 < (x + 1) * zoom; x1++) {
                    if (module_is_set(&zs, x, y)) {
                        setLedXY(x0 + x1, y0 + y1, 0, 0, 0);
                    } else {
                        setLedXY(x0 + x1, y0 + y1, 255, 255, 255);
                    }
                }
            }
        }
    }
        
    time++;
    return 0;
}

static void constructor(void) CONSTRUCTOR_ATTRIBUTES
    void constructor(void) {
        registerAnimation("QR Clock", init,tick,deinit, 1000, 2000);
}
