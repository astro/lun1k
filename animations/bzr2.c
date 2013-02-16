/* Based on http://mbostock.github.com/protovis/ex/bzr.js */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "main.h"
#include "libs/armmath.h"
#include "libs/text.h"


#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static uint8_t *bzr_a, *bzr_b, *bzr_c;
static uint8_t *t_bzr_a, *t_bzr_b, *t_bzr_c;

static uint32_t second_start;
static uint32_t second_frames;
static uint16_t fps = 0;

static void init(void) {
	bzr_a = malloc(LED_WIDTH * LED_HEIGHT * sizeof(*bzr_a) / 4);
	bzr_b = malloc(LED_WIDTH * LED_HEIGHT * sizeof(*bzr_b) / 4);
	bzr_c = malloc(LED_WIDTH * LED_HEIGHT * sizeof(*bzr_c) / 4);
	t_bzr_a = malloc(LED_WIDTH * LED_HEIGHT * sizeof(*t_bzr_a) / 4);
	t_bzr_b = malloc(LED_WIDTH * LED_HEIGHT * sizeof(*t_bzr_b) / 4);
	t_bzr_c = malloc(LED_WIDTH * LED_HEIGHT * sizeof(*t_bzr_c) / 4);

	for(int y = 0, p = 0; y < LED_HEIGHT / 2; y++) {
		for (int x = 0; x < LED_WIDTH / 2; x++, p++) {
			bzr_a[p] = rand() & 0xFF;
			bzr_b[p] = rand() & 0xFF;
			bzr_c[p] = rand() & 0xFF;
		}
	}

        second_start = 0;
        second_frames = 0;
}

static void deinit(void) {
	free(bzr_a);
	free(bzr_b);
	free(bzr_c);
	free(t_bzr_a);
	free(t_bzr_b);
	free(t_bzr_c);
}

static void bzr_line(int y) {
        int p = y * LED_WIDTH / 2;
        for (int x = 0; x < LED_WIDTH / 2; x++, p++) {
                /* Compute neighbor averages, with wrap-around. */
                int16_t sa = 0, sb = 0, sc = 0;
                for(int j = y - 2 ; j < y + 3; j++) {
                        for(int i = x - 2; i < x + 3; i++) {
                                int q =
#if LED_WIDTH == 128 && LED_HEIGHT == 128
                                        (j & 0x3f) * (LED_WIDTH / 2) + (i & 0x3f);
#else
                                (j < 0 ? j + LED_HEIGHT : j >= LED_HEIGHT ? j - LED_HEIGHT : j) * LED_WIDTH +
                                        (i < 0 ? i + LED_WIDTH : i >= LED_WIDTH ? i - LED_WIDTH : i);
#endif
                                sa += bzr_a[q];
                                sb += bzr_b[q];
                                sc += bzr_c[q];
                        }
                }


                sa /= 25;
                sb /= 25;
                sc /= 25;

                int16_t ta = (sa * (259 + sb - sc)) >> 8;
                int16_t tb = (sb * (259 + sc - sa)) >> 8;
                int16_t tc = (sc * (259 + sa - sb)) >> 8;
                t_bzr_a[p] = MIN(ta,255);
                t_bzr_b[p] = MIN(tb,255);
                t_bzr_c[p] = MIN(tc,255);
        }
}

static void bzr_sync(void) {
	for(int y = 0, p = 0; y < LED_HEIGHT / 2; y++) {
		for (int x = 0; x < LED_WIDTH / 2; x++, p++) {
			bzr_a[p] = t_bzr_a[p];
			bzr_b[p] = t_bzr_b[p];
			bzr_c[p] = (t_bzr_a[p]>>4)+(t_bzr_c[p]);
		}
	}
}

static uint8_t tick(void) {
	static int time = 0;
	time++;

        uint32_t start = getSysTick();
        if (start - 10000 >= second_start) {
            fps = second_frames;
            second_frames = 0;
            second_start = start;
        }
        second_frames++;

	for(int y = 0; y < LED_HEIGHT / 2; y++) {
                bzr_line(y);
	}    
        bzr_sync();

        float a = 2 * M_PI * sin(2 * M_PI * time / 2000.0f);
        float sin_f = sin(a);
        float cos_f = cos(a);
        float sin_time = sin(2 * M_PI * time / 200.0f);
        for(int y = 0; y < LED_HEIGHT; y++) {
                for(int x = 0; x < LED_WIDTH; x++) {
                        /* Rotate */
                        float px = (float)x * cos_f - (float)y * sin_f;
                        float py = (float)x * sin_f + (float)y * cos_f;
                        /* Scale */
                        px *= 1 + 0.4 * sin_time;
                        py *= 1 + 0.3 * sin_time;
                        int p = ((int)px & 0x3f) + ((int)py & 0x3f) * LED_WIDTH / 2;
			setLedXY(x, y, 
				sini(time*50+50*t_bzr_a[p])>>8,
				sini(0x1555+time*70+30*t_bzr_b[p])>>8,
				sini(0x2aaa+time*30+20*t_bzr_c[p])>>8
                          );
                }
        }

        char frametime[16];
        snprintf(frametime, 15, "%i F/s", fps);
        draw_text_8x6(LED_WIDTH - 6 * strlen(frametime), LED_HEIGHT - 12, frametime, 255, 255, 255);

	return 0;
}

static void constructor(void) CONSTRUCTOR_ATTRIBUTES
	void constructor(void) {
        registerAnimation("bzr2",init,tick,deinit, 20, 2000);
}
