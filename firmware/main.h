#ifndef MAIN_H_
#define MAIN_H_

#include "stm32f4xx.h"
#include "drivers/ssd1351.h"
#include "libs/mcp.h"

void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);
uint32_t getSysTick(void);

#define LUN1K

#define KEY_A (1<<0)
#define KEY_ESC (1<<1)
#define KEY_STICK (1<<2)
#define KEY_B (1<<3)

uint16_t get_key_press( uint16_t key_mask );
uint16_t get_key_state( uint16_t key_mask );

#define CONSTRUCTOR_ATTRIBUTES	__attribute__((constructor));

#define CCM_ATTRIBUTES	__attribute__ ((section(".ccm")));

#endif
