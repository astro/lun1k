/**
 * Master Control Program
 **/

#include <stdlib.h>

#include "main.h"

#define MAX_ANIMATIONS 30

static int animationcount = 0;



struct animation {
	init_fun init_fp;
	tick_fun tick_fp;
	deinit_fun deinit_fp;
	uint16_t duration;
	uint16_t timing;
};

static struct animation animations[MAX_ANIMATIONS] CCM_ATTRIBUTES;



void registerAnimation(init_fun init,tick_fun tick, deinit_fun deinit,uint16_t t, uint16_t count)
{
	if(animationcount == MAX_ANIMATIONS)
		return;
	animations[animationcount].init_fp = init;
	animations[animationcount].tick_fp = tick;
	animations[animationcount].deinit_fp = deinit;
	animations[animationcount].duration = count;
	animations[animationcount].timing = t;

	animationcount++;
}

static int current_animation = 0;
static struct animation *animation = NULL;
static int tick_count = 0;

void mcp_tick(void)
{
        if (!animation) {
                animation = &animations[current_animation];
                animation->init_fp();
        }

        tick_count++;
        animation->tick_fp();
        
        if(get_key_press(KEY_ESC))
        {
                animation->deinit_fp();
                animation = NULL;

                current_animation++;
                if(current_animation == animationcount)
                {
                        current_animation = 0;
                }
                tick_count=0;
	
                lcdFillRGB(0,0,0);
        }
}
