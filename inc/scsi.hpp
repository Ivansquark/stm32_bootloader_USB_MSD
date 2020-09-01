#ifndef SCSI_HPP
#define SCSI_HPP
#include "stdint.h"

#include "usb_device.hpp"
#include "flash.hpp"

class SCSI
{
public:
	SCSI();
	static bool recieveCommandFlag;
	static bool transiveFifoFlag;
	static bool bulkFifoFlag;
	//uint8_t buf[512];
	uint8_t buf2K[2048]={0};
	enum SCSI_NAME
	{
		/*!выяснения степени готовности устройства к работе.*/
		/*!<Команда "TEST UNIT READY" используется хостом для выяснения степени готовности устройства к работе.
			Команда не предусматривает этапа пересылки данных.
			Если устройство готово, то оно возвращает контейнер состояния CSW со значением поля "bCSWStatus" равным "0x00".
			Если носитель не готов, устройство обновляет подробные данные о состоянии и возвращает контейнер состояния (CSW)
			со значением поля "bCSWStatus" равным "0x01" (ошибка исполнения). */
		TEST_UNIT_READY=0x00,
		/*!Если хост принял CSW с полем bCSWStatus = 1, он может послать команду REQUEST_SENSE, чтобы запросить пояснительные данные (SENSE DATA).*/
		REQUEST_SENSE = 0x03, 			
		/*!< Эта команда запрашивает структуру с информацией об устройстве.
			12 00 00 00 24 00 (EVPD и CMDDT равны 0), иначе ответ ошибка в CSW */
		INQUIRY = 0x12, 						
		MODE_SENSE_6 = 0x1A,     
		/*!<Команда "PREVENT ALLOW MEDIUM REMOVAL" разрешает или запрещает извлечение носителя из устройства.
			2-х битовое поле "PREVENT" команды устанавливается в состояние "00b" для разрешения или в состояние "01b" для запрета извлечения.
			Данная команда не подразумевает этап пересылки данных.>*/	
		PREVENT_ALLOW_MEDIUM_REMOVAL  = 0x1E, 
		READ_FORMAT_CAPACITIES = 0x23, 
		/*! READ_CAPACITY_10 Используется, для того чтобы определить объем памяти устройства.
			На этапе пересылки данных устройство возвращает структуру,
			содержащую логический адрес (LBA) последнего блока на носителе и размер блока в байтах.
			Отметим, что команда запрашивает логический адрес (LBA) последнего блока, а не количество блоков на носителе.
			Логический адрес первого блока равен нулю, таким образом,
			логический адрес последнего блока на единицу меньше количества блоков.*/
		READ_CAPACITY_10 = 0x25, 
		READ_10 = 0x28,                        
		WRITE_10 = 0x2A
	};
	
	const uint8_t inquiry[36] = 
	{		
        0x00,           //Block device
        0x80,           //Removable media
        0x04,           //SPC-2
        0x02,           //Response data format = 0x02
        0x1F,           //Additional_length = length - 5
        0x00,
        0x00,
        0x00,
        'O', 'P', 'A', ' ', 'i', 'n', 'c', '.', //8
        'M', 'a', 's', 's', ' ', 'S', 't', 'o', 'r', 'a', 'g', 'e', ' ', ' ', ' ', ' ', //16
        '0', '0', '0', '1' //4
	};
	const uint8_t sense_data[18] = 
	{
			0x70,       //VALID = 1, RESPONSE_CODE = 0x70
			0x00,
			0x05,       //S_ILLEGAL_REQUEST
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	const uint8_t capacity[8] = 
	{
			0x00, 0x00, 0x00, 0x00, //Addr last blocks =  4*512- 1 =2047 bytes
			0x00, 0x00, 0x08, 0x00      //Size blocks = 512 bytes
	};
	const uint8_t mode_sense_6[4] = 
	{
			0x03, 0x00, 0x00, 0x00,
	};
	/*!Command transport 31 byte*/
	typedef struct cbw
	{
		uint32_t dCBWSignature;				//Число 0x43425355
		uint32_t dCBWTag;					//Число, которое должно совпасть со значением поля "dCSWTag" в ответном контейнере состояния команды (CSW)
		uint32_t dCBWDataTransferLength;	//Объём информации, передаваемой на этапе пересылки данных, в байтах
		uint8_t bmCBWFlags;				//Направление передачи [7]=0 - OUT [7]=1 - IN
		uint8_t bCBWLUN;				//0 для одного накопителя
		uint8_t bCBWCBLength;			//длина команды 1..16 bytes
		uint8_t CBWCB[16];				//командный блок - передаются команды CBW
	}scsi_cbw_t;
	/*!Status transport 13 bytes*/
	#pragma pack(push,1)
	typedef struct csw
	{
		uint32_t dCSWSignature;		//Число 0x53425355
		uint32_t dCSWTag;			//Число из поля "dCBWTag" принятого командного блока (CBW)
		uint32_t dCSWDataResidue;	//Разница между dCBWDataTransferLength и реально обработанными данными
		uint8_t bCSWStatus;			//0x00 = успешное выполнение. 0x01 = ошибка исполнения. 0x02 = ошибка протокольной последовательности.
	}scsi_csw_t;
	#pragma pack(pop)
	
	scsi_csw_t CSW = {
		0x53425355,
		//0x55534253,
		0,
		0,
		0
	};	
	void SCSI_Execute(uint8_t ep_number);
};

#endif //SCSI_HPP
