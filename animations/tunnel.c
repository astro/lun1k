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

#include "lib/tunnel_tex.h"

#define MIN(a, b) (a < b ? a : b)


static uint16_t **zTable;

/* No idea why the one in math.h doesn't work */
static double pythagoras1( double side1, double side2 )
{
	return sqrt(side1*side1+side2*side2);
}

static void init(void) {
    int16_t x, y;
		
    zTable = malloc(LED_HEIGHT * sizeof(uint16_t *));

    for(y = 0; y < LED_HEIGHT; y++) 
    {
        zTable[y] = malloc((y+1)*sizeof(uint16_t));
        for(x = 0; x < (y+1); x++) 
        {
            int16_t
                x1 = x - LED_WIDTH,
                y1 = y - LED_HEIGHT;
            double distance =
                1 /
                (pythagoras1(x1, y1)
                 + 1);
            uint16_t z = MIN(0xfff, 0xfff * distance);
            zTable[y][x] = z;
        }
    }
}

static uint16_t getA(uint16_t x,uint16_t y)
{
            int16_t
                x1 = x - LED_WIDTH,
                y1 = y - LED_HEIGHT;
            float angle = atan2f(x1, y1);
            return 0x1fff + (angle * 0x1fff) / 3.1514f;

}

static uint16_t getZ(uint16_t x, uint16_t y)
{

    if(x >= LED_WIDTH)
        x = (LED_WIDTH - 1)-(x-(LED_WIDTH));
    if(y >= LED_HEIGHT)
        y = (LED_HEIGHT - 1)-(y-(LED_HEIGHT));
    if(y > x)
    {
        uint8_t t = x;
        x = y;
        y = t;
    }
    return zTable[x][y];
}

static void deinit(void) {

	// free 
	for(int y = 0; y < LED_HEIGHT; y++) 
    {
		free(zTable[y]);
	}
	free(zTable);
}


static uint32_t __attribute__((always_inline)) getTex(uint16_t a, uint16_t z) {
    return texGetRGB(z, a >> 6);
}


// as this is a "screensaver" you should display the message/nick somewhere

static uint8_t tick(void) {
    static uint16_t t = 0;
    t++;
    int16_t shiftLookX = LED_WIDTH * (uint32_t)sini(49 * t + 0x0fff) / 0xffff;
    int16_t shiftLookY = LED_HEIGHT * (uint32_t)sini(31 * t + 0x0fff) / 0xffff;
	
    uint16_t x, y;

    for(y = 0; y < LED_HEIGHT; y++) 
    {
        for(x = 0; x < LED_WIDTH; x++) 
        {
            uint8_t x1 = x + shiftLookX;
            uint8_t y1 = y + shiftLookY;
            uint16_t z = getZ(x1,y1);
            uint16_t a = getA(x1,y1);
            uint32_t texel = getTex(a + t * 0x7, z + t * 2);
            uint8_t r = ((texel & 0xff0000) >> 16);
            uint8_t g = ((texel & 0xff00) >> 8);
            uint8_t b = (texel & 0xff);
            /* apply shade */
            uint16_t f = 0xff - MIN(0xff, z);
            r = f * r >> 8;
            g = f * g >> 8;
            b = f * b >> 8;
            
            setLedXY(x, y, r, g, b);
        }
    }

    return 0;
}

static void constructor(void) CONSTRUCTOR_ATTRIBUTES
void constructor(void) {
        registerAnimation("Tunnel", init,tick,deinit, 4, 1000);
}




