/* Based on: http://jsfiddle.net/uzMPU/ */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "main.h"
#include "libs/armmath.h"


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


uint32_t fpsin(uint16_t x) {
    return sini(x) * 2 - 0xffff;
}

uint32_t fpcos(uint16_t x) {
    return cosi(x) * 2 - 0xffff;
}

uint8_t clip(int32_t a) {
    if (a < 0)
        return 0;
    if (a > 255)
        return 255;
    else
        return a;
}

#define FPMUL(a, b) ((((a) >> 8) * ((b) >> 8)))

static uint8_t tick(void) {
    int32_t xRot = ((fpsin(getSysTick()) >> 4) >> 4) + 0x3fff;
    int32_t yRot = fpcos(getSysTick() >> 4) >> 4;
    /* printf("xRot=%08X\tyRot=%08X\n", xRot, yRot); */
    int32_t yCos = fpcos(yRot >> 2);
    int32_t ySin = fpsin(yRot >> 2);
    int32_t xCos = fpcos(xRot >> 2);
    int32_t xSin = fpsin(xRot >> 2);
    /* printf("ySin=%08X\txCos=%08X\txSin=%08X\n", ySin, xCos, xSin); */

    int32_t o[3] = {
        0x108000 + ((getSysTick() << 4) & 0x1fffff) ,
        0x108000,
        0x108000
    };
    /* printf("o[0]=%08X\n", o[0]); */

    f++;
    for(int x = 0; x < LED_WIDTH; x++) {
        int32_t ___xd = ((x - LED_WIDTH / 2) << 16) / LED_HEIGHT;
        for(int y = 0; y < LED_HEIGHT; y++) {
            int32_t __yd = ((y - LED_HEIGHT / 2) << 16) / LED_HEIGHT;
            int32_t __zd = 0x10000;

            int32_t ___zd = FPMUL(__zd, yCos) + FPMUL(__yd, ySin);
            int32_t _d[3] = {
              FPMUL(___xd, xCos) + FPMUL(___zd, xSin),
              FPMUL(__yd, yCos) - FPMUL(__zd, ySin),
              FPMUL(___zd, xCos) - FPMUL(___xd, xSin)
            };
            /* if (x >= 60 && x < 70 && y >= 60 && y < 70) { */
            /*     printf("%ix%i\t%08Xx%08Xx%08X\n", x, y, _d[0], _d[1], _d[2]); */
            /* } */

            int32_t col = 0;
            int32_t br = 0xff0000;
            int ddist = 0;

            int32_t closest = 0x100000;
            for (int d = 0; d < 3; d++) {
                int32_t dimLength = _d[d];

                int32_t ll = 0x40000000 / ((dimLength < 0 ? -dimLength : dimLength) | 1);
                int32_t dp[3] = {
                    FPMUL(_d[0], ll),
                    FPMUL(_d[1], ll),
                    FPMUL(_d[2], ll)
                };
                /* if (x >= 60 && x < 70 && y >= 60 && y < 70) { */
                /*     printf("%ix%i\t%08Xx%08Xx%08X\n", x, y, dp[0], dp[1], dp[2]); */
                /* } */

                int32_t initial = o[d] - (o[d] & 0xffff0000);
                if (dimLength > 0)
                    initial = 0x10000 - initial;

                int32_t dist = FPMUL(ll, initial);

                int32_t p[3];
                for(int i = 0; i < 3; i++)
                    p[i] = o[i] + FPMUL(dp[i], initial);

                if (dimLength < 0) {
                    p[d] -= 0x10000;
                }

                while (dist < closest) {
                    uint8_t tex = map[((p[2] >> 16) & 31) << 10 | ((p[1] >> 16) & 31) << 5 | ((p[0] >> 16) & 31)];

                    if (tex > 0) {
                        int u = ((p[0] + p[2]) >> 12) & 0xf;
                        int v = ((p[1] >> 12) & 0xf) + 16;
                        if (d == 1) {
                            u = (p[0] >> 12) & 15;
                            v = (p[2] >> 12) & 15;
                            if (dp[1] < 0)
                                v += 32;
                        }

                        uint32_t cc = texmap[u + v * 16 + tex * 256 * 3];
                        if (cc > 0) {
                            /* printf("%ix%i\t%ix%ix%i\tdist=%i\n", x, y, p[0] >> 16, p[1] >> 16, p[2] >> 16, dist >> 16); */
                            /* printf("%ix%i\t%08Xx%08Xx%08X\tdist=%08X\n", x, y, p[0], p[1], p[2], dist); */
                            col = cc;
                            ddist = 255 - (dist >> 12);
                            br = 255 - ((d + 2) % 3) * 50;
                            closest = dist;
                            break;
                        }
                    }
                    for(int i = 0; i < 3; i++)
                        p[i] += dp[i];
                    dist += ll;
                }
            }

            uint8_t r = clip((((col >> 16) & 0xff) * br * ddist) >> 16);
            uint8_t g = clip((((col >> 8) & 0xff) * br * ddist) >> 16);
            uint8_t b = clip((((col) & 0xff) * br * ddist) >> 16);// + (255 -
            setLedXY(x, y, r, g, b);
        }
    }

    return 0;
}

static void constructor(void) CONSTRUCTOR_ATTRIBUTES
void constructor(void) {
        registerAnimation("Notch (float)", init,tick,deinit, 0, 1000);
}
