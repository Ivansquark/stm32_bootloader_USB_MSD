/*!
 * \file main file with main function from which program is started after initialization procedure in startup.cpp
 * 
 */
#include "main.h"

void *__dso_handle = nullptr; // dummy "guard" that is used to identify dynamic shared objects during global destruction. (in fini in startup.cpp)

static constexpr uint32_t last_page= 0x0803F800; // - address of last flash 2k page (2k=0x800, 1k=0x400)

SCSI scsi; //выделение памяти под объект с буффером 2K на одну страницу флэша в оперативной памяти (2048)
Flash flash;
QueT<uint8_t,2048> que;//TODO: удалить

bool BootState()
{	
	RCC->APB2ENR|=RCC_APB2ENR_IOPCEN;
	GPIOC->CRH&=~GPIO_CRL_CNF3;
	GPIOC->CRH&=~GPIO_CRL_MODE3;
	GPIOC->ODR|=GPIO_ODR_ODR3;
	bool x =GPIOC->IDR & GPIO_IDR_IDR3; 
	return (!x); //если есть питание на пине
}	

int main()
{		    
	if (!BootState())
	{//! входим в bootloader
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
		while(1)
		{
			if(scsi.recieveCommandFlag)
			{
				//USART_debug::usart2_sendSTR("\n Execute \n");
				scsi.SCSI_Execute(2); //обработка протокола SCSI лежащего в приемном буффере устройства				
				//scsi.recieveCommandFlag=false;
			}	
			else
			{
				//font16.intToChar(USB_DEVICE::pThis->resetFlag);
				//font16.print(10,100,0x00ff,font16.arr,3);
				//font16.intToChar(USB_DEVICE::pThis->counter);
				//font16.print(100,100,0x00ff,font16.arr,3);				
			}
		}
	}
	else //! идем в код с адреса 0x08005000
	{
		//NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x08005000); //смещаем таблицу векторов прерываний на новый адрес 
		flash.goToUserApp();
		/*!< необходимо в скрипте линкера новой прошивки указывать 
			 адрес старта программы (секция кода ROM или FLASH) с 0x08005000
			 начиная с этого адреса и будет располагаться таблица векторов прерывания>*/
	}	
    return 0;
}
