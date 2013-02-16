#include <stdlib.h>
#include "main.h"
#include <stdio.h>
#include <math.h>
#include "libs/armmath.h"
//#include "lib/sektor.h"

static uint16_t a = 0;

static uint8_t tick(void) {

	
	float x0 = sini(a*40)/512.0f; 
	float y0 = sini((a*40)+0x1000)/512.0f;
	float x1 = sini(a*40)/512.0f;
	float y1 = sini((a*80)+0x2000)/512.0f;
	uint8_t x, y;

	for(y = 0; y < LED_HEIGHT; y++) 
	{
		for(x = 0; x < LED_WIDTH; x++) 
		{
			int dist = sini(pythagorasf(x0-x,y0-y)*(  215+ (sini(a*300)>>7)   ))>>9;
			int dist2 = sini(pythagorasf(x1-x,y1-y)*512)>>9 ;
				// sini((dist*dist2*dist2)+a*300)>>8, -> noise


			setLedXY(
				x,y,
				sini((dist*dist2)+a*170)>>8,
				sini((dist*dist2)+a*200)>>8,
				sini((dist*dist2)+a*260)>>8

			);
				//(getImg(y,(x+a)%153)/2)+(sini((dist*dist2)+a*300)>>13)
				//getImg(y,x)+sini((dist*dist2)+a*300)>>12
		}
	}
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
        registerAnimation("Interference", init,tick,deinit, 4, 1638); 
}




