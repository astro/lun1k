/* Based on: http://jsfiddle.net/uzMPU/ */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "main.h"


uint8_t *map;
uint32_t *texmap;

static void init(void) {
    map = malloc(32 * 32 * 32 * sizeof(*map));
    texmap = malloc(16 * 16 * 3 * 16 * sizeof(*texmap));

    for(int i = 1; i < 16; i++) {
        int  br = 255 - ((float)rand() * 96 / RAND_MAX);
        for(int y = 0; y < 16 * 3; y++) {
            for (int x = 0; x < 16; x++) {
                uint32_t color = 0xff9e50;
                if (i == 4)
                    color = 0x7F7F7F;
                if (i != 4 || (rand() % 3 == 0)) {
                    br = 255 - ((float)rand() * 96 / RAND_MAX);
                }
                if ((i == 1 && y < (((x * x * 3 + x * 81) >> 2) & 3) + 18)) {
                    color = 0x8bff3e;
                } else if ((i == 1 && y < (((x * x * 3 + x * 81) >> 2) & 3) + 19)) {
                    br = br * 2 / 3;
                }
                if (i == 7) {
                    color = 0xb88f4f;
                    if (x > 0 && x < 15
                        && ((y > 0 && y < 15) || (y > 32 && y < 47))) {
                        
                        color = 0xd0bc9f;
                        int xd = (x - 7);
                        int yd = ((y & 15) - 7);
                        if (xd < 0)
                            xd = 1 - xd;
                        if (yd < 0)
                            yd = 1 - yd;
                        if (yd > xd)
                            xd = yd;

                        br = 196 - (rand() & 31) + xd % 3 * 32;
                    } else if (rand() & 1) {
                        br = br * (150 - (x & 1) * 100) / 100;
                    }
                }

                if (i == 5) {
                    color = 0xff4911;
                    if ((x + (y >> 2) * 4) % 8 == 0 || y % 4 == 0) {
                        color = 0xe0d3c9;
                    }
                }
                if (i == 9) {
                    color = 0x5656ff;
                }
                int brr = br;
                if (y >= 32)
                    brr /= 2;

                if (i == 8) {
                    color = 0x5eff3e;
                    if (rand() & 1) {
                        color = 0;
                        brr = 255;
                    }
                }

                uint32_t col =
                    (((color >> 16) & 0xff) * brr / 255) << 16 |
                    (((color >> 8) & 0xff) * brr / 255) << 8 |
                    (((color) & 0xff) * brr / 255);
                texmap[x + y * 16 + i * 256 * 3] = col;
            }
        }
    }

    for(int x = 0; x < 32; x++) {
        for(int y = 0; y < 32; y++) {
            for (int z = 0; z < 32; z++) {
                int i = z << 10 | y << 5 | x;
                float yd = (y - 16.5) * 0.4;
                float zd = (z - 16.5) * 0.4;
                map[i] = rand() & 0xF;
                if ((float)rand() / RAND_MAX > sqrt(sqrt(yd * yd + zd * zd)) - 0.8)
                    map[i] = 0;
            }
        }
    }
}

static void deinit(void) {
    free(map);
    free(texmap);
}


static int f = 0;

static uint8_t tick(void) {
    float xRot = sinf((float)(getSysTick() % 100000) / 100000 * M_PI * 2) * 0.4
        + M_PI / 2;
    float yRot = cosf((float)(getSysTick() % 100000) / 100000 * M_PI * 2) * 0.4;
    float yCos = cosf(yRot);
    float ySin = sinf(yRot);
    float xCos = cosf(xRot);
    float xSin = sinf(xRot);

    float o[3] = {
        16.5 + (float)(getSysTick() % 100000) / 100000 * 64,
        16.5,
        16.5
    };

    f++;
    int pw, ph;
    if (f & 1) {
      pw = 2;
      ph = 1;
    } else {
      pw = 1;
      ph = 2;
    }
    for(int x = 0; x < LED_WIDTH; x += pw) {
        float ___xd = (float)(x - LED_WIDTH / 2) / LED_HEIGHT;
        for(int y = 0; y < LED_HEIGHT; y += ph) {
            float __yd = (float)(y - LED_HEIGHT / 2) / LED_HEIGHT;
            float __zd = 1;

            float ___zd = __zd * yCos + __yd * ySin;
            float _d[3] = {
                ___xd * xCos + ___zd * xSin,
                __yd * yCos - __zd * ySin,
                ___zd * xCos - ___xd * xSin
            };

            uint32_t col = 0;
            float br = 255;
            int ddist = 0;

            float closest = 20;
            for (int d = 0; d < 3; d++) {
                float dimLength = _d[d];

                float ll = 1 / (dimLength < 0 ? -dimLength : dimLength);
                float dp[3] = {
                    (_d[0]) * ll,
                    (_d[1]) * ll,
                    (_d[2]) * ll
                };

                float initial = o[d] - (int)o[d];
                if (dimLength > 0)
                    initial = 1 - initial;

                float dist = ll * initial;

                float p[3];
                for(int i = 0; i < 3; i++)
                    p[i] = o[i] + dp[i] * initial;

                if (dimLength < 0) {
                    p[d]--;
                }

                while (dist < closest) {
                    uint8_t tex = map[((int)p[2] & 31) << 10 | ((int)p[1] & 31) << 5 | ((int)p[0] & 31)];

                    if (tex > 0) {
                        int u = (int)((p[0] + p[2]) * 16) & 15;
                        int v = ((int)(p[1] * 16) & 15) + 16;
                        if (d == 1) {
                            u = (int)(p[0] * 16) & 15;
                            v = ((int)(p[2] * 16) & 15);
                            if (dp[1] < 0)
                                v += 32;
                        }

                        uint32_t cc = texmap[u + v * 16 + tex * 256 * 3];
                        if (cc > 0) {
                            col = cc;
                            ddist = 255 - (int)(dist / 32 * 255);
                            br = 255 - ((d + 2) % 3) * 50;
                            closest = dist;
                        }
                    }
                    for(int i = 0; i < 3; i++)
                        p[i] += dp[i];
                    dist += ll;
                }
            }

            uint8_t r = ((col >> 16) & 0xff) * br * ddist / (255 * 255);
            uint8_t g = ((col >> 8) & 0xff) * br * ddist / (255 * 255);
            uint8_t b = ((col) & 0xff) * br * ddist / (255 * 255);// + (255 -
            setLedXY(x, y, r, g, b);
            if (pw == 2)
              setLedXY(x + 1, y, r, g, b);
            if (ph == 2)
              setLedXY(x, y + 1, r, g, b);
        }
    }
    return 0;
}

static void constructor(void) CONSTRUCTOR_ATTRIBUTES
void constructor(void) {
        registerAnimation("Notch", init,tick,deinit, 0, 1000);
}
