#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "main.h"
#include "libs/text.h"
#include "libs/armmath.h"
#include "libs/newvox.h"

static int done;
static int k;
static float ss,sa,a,s;
static int x0,y0_;

static void init(void) {
    /* Compute the height map */
    ComputeMap();

    /* Main loop
    
       a     = angle
       x0,y0_ = current position
       s     = speed constant
       ss    = current forward/backward speed
       sa    = angular speed
    */
    done=0;
    a=0; k=x0=y0_=0;
    s=512;
    ss=5;
    sa=0;
}

static void deinit(void) {
    FreeMap();
}


static uint8_t tick(void) {
    /* Draw the frame */
    View(x0,y0_,a);

    /* Update position/angle */
    x0+=ss*cos(a); y0_+=ss*sin(a);
    a+=sa;

    static float a = 0;
    a += 0.01;
    sa = 0.03 * sin(a * M_PI);
    ss = 20 * s;

	return 0;
}



static void constructor(void) CONSTRUCTOR_ATTRIBUTES
    void constructor(void) {
        registerAnimation("Voxel", init,tick,deinit, 4, 1000);
}
