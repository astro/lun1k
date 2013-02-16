/* Based on: http://lodev.org/cgtutor/tunnel.html */

/*
 *
 * this version uses only about 8k ram (184k less than tunnel.c)
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "main.h"
#include "libs/text.h"
#include "libs/armmath.h"


static uint16_t **cTable;
static uint16_t **aTable;

static float fast_atan2( float y, float x )
{
	const float ONEQTR_PI = M_PI / 4.0;
	const float THRQTR_PI = 3.0 * M_PI / 4.0;
	float r, angle;
	float abs_y = fabs(y) + 1e-10f;      // kludge to prevent 0/0 condition
	if ( x < 0.0f )
	{
		r = (x + abs_y) / (abs_y - x);
		angle = THRQTR_PI;
	}
	else
	{
		r = (x - abs_y) / (x + abs_y);
		angle = ONEQTR_PI;
	}
	angle += (0.1963f * r * r - 0.9817f) * r;
	if ( y < 0.0f )
		return( -angle );     // negate if in quad III or IV
	else
		return( angle );
}

static void init(void) {
    double fact = 0xffff / pythagorasf(LED_WIDTH / 2, LED_HEIGHT / 2);
    int16_t x, y;
		
    cTable = malloc(LED_WIDTH / 2 * sizeof(uint16_t *));

    for(y = 0; y < LED_HEIGHT / 2; y++) 
    {
        cTable[y] = malloc((y+1)*sizeof(uint16_t));
        for(x = 0; x < (y+1); x++) 
        {
            double
                x1 = x - LED_WIDTH / 2,
                y1 = y - LED_HEIGHT / 2;
            cTable[y][x] = fact * pythagorasf(x1, y1);
        }
    }
    aTable = malloc(LED_WIDTH / 2 * sizeof(uint16_t *));
    for(y = 0; y < LED_HEIGHT / 2; y++) 
    {
        aTable[y] = malloc(LED_WIDTH / 2*sizeof(uint16_t));
        for(x = 0; x < LED_WIDTH / 2; x++) 
        {
            aTable[y][x] = 0x7fff * (fast_atan2(x,-y) / M_PI + 1);
        }
    }
}

static uint16_t getC(uint16_t x, uint16_t y)
{
    if(x >= LED_WIDTH / 2)
        x = (LED_WIDTH / 2 - 1)-(x-(LED_WIDTH / 2));
    if(y >= LED_HEIGHT / 2)
        y = (LED_HEIGHT / 2 - 1)-(y-(LED_HEIGHT / 2));
    if(y > x)
    {
        uint8_t t = x;
        x = y;
        y = t;
    }
    return cTable[x][y];
}


static uint16_t getA(int x, int y)
{
    while(x <= -LED_WIDTH / 2 || x >= LED_WIDTH / 2 ||
          y <= -LED_HEIGHT / 2 || y >= LED_HEIGHT / 2)
        x /= 2, y /= 2;

    /* printf("getA %i %i\n", x, y); */
    if (x >= 0 && y <= 0)
        return aTable[x][-y];
    else if (x >= 0)
        return 0x2000 - aTable[x][y];
    else if (x <= 0 && y >= 0)
        return aTable[-x][y] + 0x2000;
    else
        return 0x4000 - aTable[-x][-y];
}

static void getPlasma(int a, int x, int y, uint8_t *r, uint8_t *g, uint8_t *b) {
    a &= 0x3fff;
    static int prev_a = -1;

    static uint16_t sin1;
    static float x0;
    static float y0;
    static float x1;
    static float y1;
    if (a != prev_a) {
        sin1 = sini(a);
        x0 = (float)sini(a*4)/256-64;
        y0 = (float)sini((a*4)+0x1000)/256-64;
        x1 = (float)sini(a*5)/128-128;
        y1 = (float)sini((a*5)+0x1000)/128-128;
    }

    uint16_t y_part =  sini(sin1+y*20);


    float dist = pythagorasf(x0-x,y0-y);
    float dist2 = pythagorasf(y1-x,x1-y);

    uint16_t h = sini(sin1+x*20)+ y_part + sini(dist*500) + sini(dist2*300);
    uint16_t h2 = sini(sin1+x*30)+ y_part + sini(dist*200) + sini(dist2*350);

    *r = sini((h>>2)+a*500)>>8;
    *g = sini((h2>>2)+a*320+0x1555)>>8;
    *b = sini((h>>2)+a*630+0x2aaa)>>8;
}


static void deinit(void) {
    for(int y = 0; y < LED_HEIGHT; y++) 
    {
        free(cTable[y]);
        free(aTable[y]);
    }
    free(cTable);
    free(aTable);
}




static uint8_t tick(void) {
    static uint16_t t = 0;
    t++;
   
    int da = (t << 8) & 0xffff;

    uint16_t x, y;

    for(y = 0; y < LED_HEIGHT; y++) 
    {
        for(x = 0; x < LED_WIDTH; x++) 
        {
            int a = getA(x - LED_WIDTH / 2, y - LED_HEIGHT / 2) + da;
#ifdef SIMULATOR
			if (x > 61 && x < 68 && y > 61 && y < 68)
                printf("%ix%i\ta=%04X\n", x, y, a);
#endif
            int slice = a >> 13;
            int v = a >> 9;
            int c = getC(x, y);
            int w = (int)(c >> 12) + t;
            
            uint8_t r, g, b;
            if (slice & 1) {
                getPlasma(t, w, v, &r, &g, &b);
            } else {
                getPlasma(t, v, w, &r, &g, &b);
            }
            setLedXY(x, y, r, g, b);
        }
    }

    return 0;
}

static void constructor(void) CONSTRUCTOR_ATTRIBUTES
void constructor(void) {
        registerAnimation("Kaleidoscope", init,tick,deinit, 4, 1000);
}




