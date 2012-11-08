/* Based on: http://lodev.org/cgtutor/tunnel.html */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "main.h"
#include "libs/text.h"
#include "libs/math.h"

#include "lib/tunnel_tex.h"

#define MIN(a, b) ((a < b) ? a : b)


static uint16_t zTable[2 * LED_WIDTH][2 * LED_HEIGHT];
static uint16_t aTable[2 * LED_WIDTH][2 * LED_HEIGHT];

/* No idea why the one in math.h doesn't work */
static double pythagoras1( double side1, double side2 )
{
	return sqrt(side1*side1+side2*side2);
}

static void init(void) {
    int16_t x, y;
		
    for(y = 0; y < 2 * LED_HEIGHT; y++) 
    {
        for(x = 0; x < 2 * LED_WIDTH; x++) 
        {
            int16_t
                x1 = x - LED_WIDTH,
                y1 = y - LED_HEIGHT;
            double distance =
                1 /
                (pythagoras1(x1, y1)
                 + 1);

            uint16_t z = MIN(0xfff, 0xfff * distance);
            zTable[x][y] = z;

            double angle = atan2(x1, y1);
            uint16_t a = 0x1fff + (angle * 0x1fff) / M_PI;
            aTable[x][y] = a;
        }
    }
}

static void deinit(void) {

	// free 

}


static uint32_t __attribute__((always_inline)) getTex(uint16_t a, uint16_t z) {
    /* printf("getTex(%X, %X)\n", z, a); */
    return texGetRGB(z, a >> 6);
}


// as this is a "screensaver" you should display the message/nick somewhere

static uint8_t tick(void) {
    static uint16_t t = 0;
    t++;
    int16_t shiftLookX = LED_WIDTH * (uint32_t)sini(9 * t + 0x0fff) / 0xffff;
    int16_t shiftLookY = LED_HEIGHT * (uint32_t)sini(5 * t + 0x0fff) / 0xffff;
	
    uint16_t x, y;

    for(y = 0; y < LED_HEIGHT; y++) 
    {
        for(x = 0; x < LED_WIDTH; x++) 
        {
            /* TODO: delta speed from shiftLook[XY] */
            uint16_t z = zTable[x + shiftLookX][y + shiftLookY];
            uint16_t a = aTable[x + shiftLookX][y + shiftLookY];
            /* printf("%ix%i\ta=%04X\tz=%04X\n", x, y, a, z); */
            uint32_t texel = getTex(a + t * 7, z + t / 10);
            uint8_t r = ((texel & 0xff0000) >> 16);
            uint8_t g = ((texel & 0xff00) >> 8);
            uint8_t b = (texel & 0xff);
            /* apply shade */
            uint16_t f = 0x1ff - MIN(0x1ff, z);
            r = f * r >> 9;
            g = f * g >> 9;
            b = f * b >> 9;

            setLedXY(x, y, r, g, b);
        }
    }

    return 0;
}

static void constructor(void) CONSTRUCTOR_ATTRIBUTES
void constructor(void) {
    registerAnimation(init,tick,deinit, 4, 1000);
}




