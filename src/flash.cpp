#include "flash.hpp"

Flash* Flash::pThis=nullptr;

Flash::Flash()
{pThis=this;}

void Flash::erase(uint32_t pageAddr)
{
	unlock();
	while(FLASH->SR & FLASH_SR_BSY); // no flash memory operation is ongoing (BSY -busy)
	if (FLASH->SR & FLASH_SR_EOP) 
	{
		FLASH->SR = FLASH_SR_EOP; //END of operation Set by hardware when a Flash operation (programming / erase) is completed  Reset by writing a 1
	}
	FLASH->CR |= FLASH_CR_PER;//page erase chosen
	
	FLASH->AR = pageAddr;  // sets page address
	FLASH->CR |= FLASH_CR_STRT; // This bit triggers an ERASE operation when set.
	while (!(FLASH->SR & FLASH_SR_EOP)); //while not end of operation
	FLASH->SR = FLASH_SR_EOP;	//reset bit
	FLASH->CR &= ~FLASH_CR_PER; //unchose page erase
}

void Flash::write_any_buf(uint32_t addr, void* buf, uint16_t len)
{	
	for(uint32_t i=0; i<len ; i+=FLASH_PAGE_SIZE)
	{erase(addr);} //erasing flash pages
		
	while (FLASH->SR & FLASH_SR_BSY); // no flash memory operation is ongoing
	if (FLASH->SR & FLASH_SR_EOP) 
	{		FLASH->SR = FLASH_SR_EOP;	} //reset bit

	FLASH->CR |= FLASH_CR_PG; //Flash programming chosen
	/*!< half word writing>*/
	for(uint32_t i=0; i<len; i+=2)
	{
		if(i+1 != len)
		{
			*(volatile uint16_t*)(addr+i) = (*(uint16_t*)(buf+i+1)<<8) + *(uint8_t*)(buf+i);
		}
		else {*(volatile uint16_t*)(addr+i) = 0x0000 + *(uint8_t*)(buf+i);}
		while (!(FLASH->SR & FLASH_SR_EOP)); //while writing page
		FLASH->SR = FLASH_SR_EOP;	
	}
	FLASH->CR &= ~(FLASH_CR_PG);	
	lock();
}

uint32_t Flash::read32(uint32_t addr)
{
	unlock();
	return *((uint32_t*)addr);
	lock();
}

void Flash::read_buf(uint32_t addr, void* buf, uint16_t len)
{
	unlock();
	for (uint32_t i=0;i<len;i+=2)
	{
		*(uint8_t*)(buf+i) =  *(uint8_t*)(addr + i);
		*(uint8_t*)(buf+i+1) =  *(uint8_t*)(addr + i + 1);
	}
	lock();
}

void Flash::goToUserApp(void)
{
	volatile uint32_t appJumpToAddress;
	appJumpToAddress = *(volatile uint32_t*)(FLASH_PROGRAMM_ADDRESS+4); //next word address
	void (*goToApp)(void); //function pointer
	goToApp = (void (*)(void))appJumpToAddress; // cast address to function pointer
	/*!<before sets new vectors table address need to tell linker new main programm address>*/
	//SCB->VTOR = FLASH_PROGRAMM_ADDRESS; //sets vectors table to new address
	__set_MSP(*((volatile uint32_t*) FLASH_PROGRAMM_ADDRESS)); //stack pointer (to RAM) for USER app in this address	
	goToApp(); //go to start main programm	
}