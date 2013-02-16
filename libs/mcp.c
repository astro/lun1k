/**
 * Master Control Program
 **/

#include <stdlib.h>

#include "main.h"
// #include "hal/inc/hal_power.h"
#include "libs/text.h"
#include "libs/mcp.h"


#define CLAMP(val, max) if (val > max) val = max

#ifdef SIMULATOR
extern uint8_t leds[LED_HEIGHT][LED_WIDTH][4];
#else
extern uint8_t leds[LED_HEIGHT][LED_WIDTH][3] CCM_ATTRIBUTES;
#endif

static void lcdDarken(void)
{
        for(int y = 0; y < LED_HEIGHT; y++)
                for(int x = 0; x < LED_WIDTH; x++)
                        for(int i = 0; i < 3; i++)
                                CLAMP(leds[y][x][i], 191);
}


#define MAX_ANIMATIONS 30

static int animationcount = 0;



struct animation {
        char *name;
	init_fun init_fp;
	tick_fun tick_fp;
	deinit_fun deinit_fp;
	uint16_t duration;
	uint16_t timing;
};

static struct animation animations[MAX_ANIMATIONS] CCM_ATTRIBUTES;



void registerAnimation(char *name, init_fun init,tick_fun tick, deinit_fun deinit,uint16_t t, uint16_t count)
{
	if(animationcount == MAX_ANIMATIONS)
		return;
	animations[animationcount].name = name;
	animations[animationcount].init_fp = init;
	animations[animationcount].tick_fp = tick;
	animations[animationcount].deinit_fp = deinit;
	animations[animationcount].duration = count;
	animations[animationcount].timing = t;

	animationcount++;
}

static int current_animation = 0;
static struct animation *animation = NULL;

static void draw_triangle(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
        for(int y1 = y; y1 < y + 13; y1++) {
                int w = 7 - abs(y1 - y - 6);
                for(int x1 = x; x1 < x + w; x1++) {
                        setLedXY(x1, y1, r, g, b);
                }
        }
}

static void handle_menu_input(void)
{
        int previous_animation = current_animation;
        if (get_key_press(KEY_B)) {
                current_animation--;
                if (current_animation < 0)
                        current_animation = animationcount - 1;
        } else if (get_key_press(KEY_A)) {
                current_animation++;
                if (current_animation >= animationcount)
                        current_animation = 0;
        }
        if (current_animation != previous_animation) {
                animation->deinit_fp();
                animation = &animations[current_animation];
                animation->init_fp();
        }
}

static void draw_menu(void)
{
        static int menu_y = 0;
        int y = menu_y;
        static struct {
                uint8_t direction;
                int xd;
        } selection_scroll = { 0, 0 };

        for(int i = 0; i < animationcount; i++) {
                if (y > -16 && y < LED_HEIGHT) {
                        int xd = i == current_animation ? selection_scroll.xd : 0;
                        if (i == current_animation) {
                                draw_triangle(4 - xd, y + 2, 255, 255, 255);
                        }
                        draw_text_16pt(16 - xd, y, animations[i].name, 255, 255, 255);
                        if (i == current_animation) {
                                int width = get_text_width_16pt(animations[i].name) + 24 - LED_WIDTH;
                                if (width > 0) {
                                        if (selection_scroll.direction == 0) {
                                                selection_scroll.xd += 2;
                                                if (selection_scroll.xd >= width)
                                                        selection_scroll.direction = 1;
                                        } else {
                                                selection_scroll.xd -= 2;
                                                if (selection_scroll.xd < 1)
                                                        selection_scroll.direction = 0;
                                        }
                                } else
                                        selection_scroll.xd = 0;
                        }
                }
                if (i == current_animation) {
                        int scroll_diff = y - LED_HEIGHT / 2 + 10;
                        if (scroll_diff < 0)
                                menu_y -= (scroll_diff / 4) + 1;
                        else if (scroll_diff > 0)
                                menu_y -= (scroll_diff / 4) - 1;
                }
                y += 18;
        }
}

static int tick_count = 0;

void mcp_tick(void)
{
        static int menu_visible = 1;
        if (!animation) {
                animation = &animations[current_animation];
                animation->init_fp();
        }

        if(get_key_press(KEY_ESC)) {
                menu_visible = !menu_visible;
                lcdFillRGB(0, 0, 0);
        }
        if (menu_visible)
                handle_menu_input();

        tick_count++;
        animation->tick_fp();

        if (menu_visible) {
                lcdDarken();
                draw_menu();
                // draw_battery();
        }
}
