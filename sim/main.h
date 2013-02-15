#ifndef _MAIN_H
#define _MAIN_H

#include <stdint.h>
#include <stdlib.h>

#define LED_WIDTH	128
#define LED_HEIGHT	128

#define LUN1K
#define SIMULATOR

#define CCM_ATTRIBUTES  /* Hack :-) */
#define CONSTRUCTOR_ATTRIBUTES	__attribute__((constructor));

#define KEY_A (1<<0)
#define KEY_ESC (1<<1)
#define KEY_STICK (1<<2)
#define KEY_B (1<<3)

uint16_t get_key_press( uint16_t key_mask );
uint16_t get_key_state( uint16_t key_mask );

#ifndef NOMALLOCWRAP

#define malloc xmalloc
#define free xfree

void *xmalloc (size_t nSize);
void  xfree (void *pMem);

#endif

typedef void (*init_fun)(void);
typedef void (*deinit_fun)(void);
typedef uint8_t (*tick_fun)(void);

int sdlpause;

uint32_t getSysTick(void);

void registerAnimation(init_fun init,tick_fun tick,deinit_fun deinit, uint16_t t, uint16_t duration);
//void registerApp(tick_fun tick,uint16_t t);
void setLedXY(uint8_t x,uint8_t  y, uint8_t r,uint8_t g,uint8_t b);
void invLedXY(uint8_t x,uint8_t  y);
void getLedXY(uint8_t x, uint8_t y, uint8_t* red,uint8_t* green, uint8_t* blue);
void lcdFillRGB(uint8_t r,uint8_t g , uint8_t b);


#endif

