/*!
 * \file main file with main function from which program is started after initialization procedure in startup.cpp
 * 
 */
#include "main.h"

void *__dso_handle = nullptr; // dummy "guard" that is used to identify dynamic shared objects during global destruction. (in fini in startup.cpp)

static constexpr uin32_t last_page= 0x0803F800 // - address of last flash 2k page (2k=0x800, 1k=0x400)

int main()
{		
    RCCini rcc;	//! 72 MHz
	RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
	RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;
	AFIO->MAPR|=AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
	SpiLcd lcd;
	lcd.fillScreen(0xff00);
	Font_16x16 font16;
	//__enable_irq();
	USART_debug usart2(2);
	uint32_t  count=0;
	uint32_t arr[1]={0x000000ff};
	flash_write(last_page,arr,1);
	while(1)
	{
		count = flash_read(last_page);
		font16.intToChar(count);
		font16.print(10,10,0x00ff,font16.arr,2);		
	}
    return 0;
}
