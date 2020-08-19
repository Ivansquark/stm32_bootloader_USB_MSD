#ifndef FLASH_H_
#define FLASH_H_

/*!<Необходимо поменять в линкер файле адрес начала программы, чтобы прошивать с того адреса который нужен.
	А до этого адреса расположить бутлоадер, в котором будет происходить переход на нужный адрес>*/

#define FLASH_START	0x08000000
#define FLASH_DISK_SIZE 0x40000 /*256K = 262144*/
#define FLASH_PAGE_SIZE 0x800 /*2K*/
#define FLASH_PROGRAMM_ADDRESS 0x08005000 /*bootloader 20K=20480 = 0x5000*/

void __set_MSP(uint32_t topOfMainStack) __attribute__( ( naked ) );
void __set_MSP(uint32_t topOfMainStack)
{
  __ASM volatile ("MSR msp, %0\n\t"
                  "BX  lr     \n\t" : : "r" (topOfMainStack) );
}

/*!< connectivity line with 127 2kB flash pages>*/

void flash_erase(uint32_t pageAddr)
{
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

uint32_t flash_read(uint32_t addr)
{
	/*!<Flash unlock>*/
	FLASH->KEYR = 0x45670123;
	FLASH->KEYR = 0xCDEF89AB;
	return *((uint32_t*)addr);
}

/*!< write a massive of bytes >*/
void flash_write(uint32_t addr, uint8_t* data, uint16_t len)
{
	/*!<Flash unlock>*/
	FLASH->KEYR = 0x45670123;
	FLASH->KEYR = 0xCDEF89AB;
	
	for(uint32_t i=0; i<len ; i+=FLASH_PAGE_SIZE)
	{flash_erase(addr);} //erasing flash pages
	
	uint32_t i;
	while (FLASH->SR & FLASH_SR_BSY); // no flash memory operation is ongoing
	if (FLASH->SR & FLASH_SR_EOP) 
	{		FLASH->SR = FLASH_SR_EOP;	} //reset bit

	FLASH->CR |= FLASH_CR_PG; //Flash programming chosen
	/*!< half word writing>*/
	for (i = 0; i < len; i += 2) 
	{
		*(volatile uint16_t*)(addr + i) = (((uint16_t)data[i + 1]) << 8) + data[i]; //write half word // first writing low byte then high byte
		while (!(FLASH->SR & FLASH_SR_EOP)); //while writing page
		FLASH->SR = FLASH_SR_EOP; 
	}
	FLASH->CR &= ~(FLASH_CR_PG);
}


void goToUserApp(void)
{
	volatile uint32_t appJumpToAddress;
	appJumpToAddress = *(volatile uint32_t*)(FLASH_PROGRAMM_ADDRESS+4); //next word address
	void (*goToApp)(void); //function pointer
	goToApp = (void (*)(void))appJumpToAddress; // cast address to function pointer
	/*!<before sets new vectors table address need to tell linker new main programm address>*/
	SCB->VTOR = FLASH_PROGRAMM_ADDRESS; //sets vectors table to new address
	__set_MSP(*((volatile uint32_t*) FLASH_PROGRAMM_ADDRESS)); //stack pointer (to RAM) for USER app in this address	
	goToApp(); //go to start main programm	
}

#endif //FLASH_H_

