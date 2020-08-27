/*!
 * \file main file with main function from which program is started after initialization procedure in startup.cpp
 * 
 */
#include "main.h"

void *__dso_handle = nullptr; // dummy "guard" that is used to identify dynamic shared objects during global destruction. (in fini in startup.cpp)

static constexpr uint32_t last_page= 0x0803F800; // - address of last flash 2k page (2k=0x800, 1k=0x400)

#pragma pack(push,1)
typedef struct 
{
	uint8_t x;
	uint8_t y;
	uint16_t z;
}buf;
#pragma pack(pop)

SCSI scsi;
Flash flash;

	

int main()
{		
    RCCini rcc;	//! 72 MHz
	
	RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
	RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;
	AFIO->MAPR|=AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
	SpiLcd lcd;
	lcd.fillScreen(0xff00);
	Font_16x16 font16;
	USB_DEVICE usb;
	__enable_irq();
	USART_debug usart2(2);
	//uint8_t arr[4]={0x01,0x00,0x00,0x00};	
	//buf b = {0,0,1};
	//flash.write_any_buf(last_page,&b,4);
	while(1)
	{
		//flash_read_buf(last_page,&b,4);
		//font16.intToChar(b.z);
		//font16.print(10,10,0x00ff,font16.arr,2);		
		if(scsi.recieveCommandFlag)
		{
			//USART_debug::usart2_sendSTR("\n Execute \n");
			scsi.SCSI_Execute(1);			
		}		
	}
    return 0;
}
