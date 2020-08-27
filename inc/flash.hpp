#ifndef FLASH_HPP
#define FLASH_HPP

#include <stdint.h>

/*!<Необходимо поменять в линкер файле адрес начала программы, чтобы прошивать с того адреса который нужен.
	А до этого адреса расположить бутлоадер, в котором будет происходить переход на нужный адрес>*/


/*!< connectivity line with 127 2kB flash pages>*/

class Flash
{
public: 
	Flash();
	inline void unlock() __attribute__( ( naked , always_inline) )
	{
		/*!<Flash unlock>*/
		FLASH->KEYR = 0x45670123;
		FLASH->KEYR = 0xCDEF89AB;
	}
	inline void lock() __attribute__( ( naked , always_inline) )
	{
		FLASH->CR |= FLASH_CR_LOCK; /* Lock the flash back */
	}
	void erase();
	void read_buf(uint32_t addr, void* buf, uint16_t len);
	void write_any_buf(uint32_t addr, void* buf, uint16_t len);
	uint32_t read32(uint32_t addr);
	void goToUserApp(void);
	inline void __set_MSP(uint32_t topOfMainStack) __attribute__( ( naked , always_inline) )
	{
		__ASM volatile ("MSR msp, %0\n\t"
						"BX  lr     \n\t" : : "r" (topOfMainStack) );
	}
	
	static costexpr uint32_t FLASH_START = 0x08000000;
	static costexpr uint32_t FLASH_DISK_SIZE = 0x40000; /*256K = 262144*/
	static costexpr uint16_t FLASH_PAGE_SIZE = 0x800;	/*2K*/
	static costexpr uint32_t FLASH_PROGRAMM_ADDRESS = 0x08005000; /*bootloader 20K=20480 = 0x5000*/
	
};


#endif //FLASH_H_

