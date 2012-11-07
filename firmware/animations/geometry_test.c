#include <stdlib.h>
#include "main.h"
#include <stdio.h>

#include "../libs/wuline.h"
#include "../libs/circle.h"
#include "../libs/text.h"
#include "../libs/math.h"


static void init(void)
{
}
static void deinit(void)
{
}

static uint8_t tick(char* nick __attribute__((__unused__))) {

	draw_filledCircle(randr(0,128),randr(0,128),randr(10,20),randr(0,255),randr(0,255),randr(0,255));
	//draw_filledCircleSlice(randr(0,128),randr(0,128),randr(10,20),randr(0,255),randr(0,255),randr(0,255),randr(0,360),randr(0,360));
	draw_line(randr(0,128),randr(0,128),randr(0,128),randr(0,128),randr(0,255),randr(0,255),randr(0,255));
//	draw_text(randr(0,128),randr(0,128),"hallo",randr(0,255),randr(0,255),randr(0,255));


	return 0;
}



static void constructor(void) ATTRIBUTES
void constructor(void) {
	registerAnimation(init,tick,deinit, 4, 300);
}

