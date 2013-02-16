#include <stdlib.h>
#include "main.h"
#include <stdio.h>
#include <math.h>
#include "libs/text.h"
#include "libs/armmath.h"

static uint16_t a = 0;

static uint8_t tick(void) {

	
	uint8_t x, y;

	uint16_t sin1 = sini(a);
	float x0 = (float)sini(a*4)/256-64;
	float y0 = (float)sini((a*4)+0x1000)/256-64;
	float x1 = (float)sini(a*5)/128-128;
	float y1 = (float)sini((a*5)+0x1000)/128-128;
		
	for(y = 0; y < LED_HEIGHT; y++) 
	{
		uint16_t y_part =  sini(sin1+y*20);


		for(x = 0; x < LED_WIDTH; x++) 
		{
			
			float dist = pythagorasf(x0-x,y0-y);
			float dist2 = pythagorasf(y1-x,x1-y);


			uint16_t h = sini(sin1+x*20)+ y_part + sini(dist*500) + sini(dist2*300);
			uint16_t h2 = sini(sin1+x*30)+ y_part + sini(dist*200) + sini(dist2*350);
			setLedXY(
				x,y,
				sini((h>>2)+a*500)>>8,
				sini((h2>>2)+a*320+0x1555)>>8,
				sini((h>>2)+a*630+0x2aaa)>>8
			);
		}
	}
	char* nick = "lun1k";
	uint16_t text_width = get_text_width_16pt(nick);
	draw_text_inv_16pt((LED_WIDTH/2)-(text_width/2),LED_HEIGHT/2-11, nick);
	a+=1;
	if(a==0x4000)
	{
		a=0;
	}
	return 0;
}

static void init(void)
{
	a = 0;
}
static void deinit(void)
{
}



static void constructor(void) CONSTRUCTOR_ATTRIBUTES
void constructor(void) {
        registerAnimation("Plasma Big Sine", init,tick,deinit, 4, 1000);
}




