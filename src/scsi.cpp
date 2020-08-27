#include "scsi.hpp"

bool SCSI::recieveCommandFlag=false;
bool SCSI::recieveDataFlag=false;

SCSI::SCSI(){}

/*! Ф-я обработки SCSI команд, должно быть прочитано FIFO и заполнена структура*/
void SCSI::SCSI_Execute(uint8_t ep_number)
{
	uint32_t i, n;
    //uint32_t status;
    uint8_t j;	
	/* приняли данные в EP1, Натягиваем scsi_cbw на приемный буфер*/
    scsi_cbw_t *cbw = (scsi_cbw_t *)USB_DEVICE::pThis->BULK_OUT_buf;
	//Если пакет успешно принят
    if (cbw->dCBWSignature==0x43425355)
	{
	//Сразу копируем значение dCBWTag в CSW.dCSWTag
        CSW.dCSWTag = (cbw -> dCBWTag);
	//Определяем пришедшую команду
        switch (cbw -> CBWCB[0])
		{
		/*!Если INQUIRY*/
        case INQUIRY:
		USART_debug::usart2_sendSTR("\n INQUIRY \n");
		
		//Проверка битов EVPD и CMDDT
            if (cbw -> CBWCB[1] == 0)
			{
				//Передаем стандартный ответ на INQUIRY
				
                USB_DEVICE::pThis->WriteINEP(ep_number, inquiry, cbw -> CBWCB[4]);				
				USART_debug::usart2_send(cbw->CBWCB[4]);
				USART_debug::usart2_sendSTR(" standINQUIRY \n");
				//Заполняем поля CSW
                CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength) - cbw -> CBWCB[4];
				//Команда выполнилась успешно
                CSW.bCSWStatus = 0x00; 
				//Посылаем контейнер состояния
                USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13); //статус всегда ин пакет
				USART_debug::usart2_sendSTR(" statusINQUERY \n");
			} else 
			{
				USART_debug::usart2_sendSTR(" errorINQUIRY \n");
				//Заполняем поля CSW
                CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength);
				//Сообщаем об ошибке выполнения команды
                CSW.bCSWStatus = 0x01;
				//Посылаем контейнер состояния
                USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
				//Подтверждаем
                CSW.bCSWStatus = 0x00;
				//Посылаем контейнер состояния
                USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
            }
        break;
		/*!Если REQUEST_SENSE*/
		case REQUEST_SENSE:
		USART_debug::usart2_sendSTR("\n REQUEST_SENSE \n");
		//Отправляем пояснительные данные
        USB_DEVICE::pThis->WriteINEP(ep_number, sense_data, 18);
		//Заполняем поля CSW
        CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength) - cbw -> CBWCB[4];
		//Команда выполнилась успешно
        CSW.bCSWStatus = 0x00;
		//Посылаем контейнер состояния
        USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
        break;
		/*! Если READ_CAPACITY_10*/
		case READ_CAPACITY_10:
		USART_debug::usart2_sendSTR("\n READ_CAPACITY_10 \n");
		//Передаем структуру
        USB_DEVICE::pThis->WriteINEP(ep_number, capacity, 8);
		//Заполняем и передаем CSW
        CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength) - cbw -> CBWCB[4];
        CSW.bCSWStatus = 0x00;
        USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
        break;
		/*!--MODE_SENSE_6--*/
		case MODE_SENSE_6:
		USART_debug::usart2_sendSTR("\n MODE_SENSE_6 \n");
		USB_DEVICE::pThis->WriteINEP(ep_number, mode_sense_6, 4);
		CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength) - cbw -> CBWCB[4];
		CSW.bCSWStatus = 0x00;
		USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
		break;
		case READ_10:
		USART_debug::usart2_sendSTR("\n READ_10 \n");
		//записываем в I начальный адрес читаемого блока
		i = ((cbw -> CBWCB[2] << 24) | (cbw -> CBWCB[3] << 16) | (cbw -> CBWCB[4] << 8) | (cbw -> CBWCB[5]));
		//записываем в n адрес последнего читаемого блока
		n = i + ((cbw -> CBWCB[7] << 8) | cbw -> CBWCB[8]);
		//выполняем чтение и передачу блоков
		for ( ; i < n; i++)
		{
			//Читаем блок из FLASH, помещаем в массив uint8_t buf[512]
			Flash::pThis->read_buf(i, buf, 512);
			//Так как размер конечной точки 64 байта, передаем 512 байт за 8 раз
			for (j = 0; j < 8; j++)
			{
				//Передаем часть буфера
				USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&buf[64*j], 64);
			}
		}
		//Заполняем и посылаем CSW
		CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength) - cbw -> CBWCB[4];
		CSW.bCSWStatus = 0x00;
		USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
		break;
		
		case WRITE_10:
		USART_debug::usart2_sendSTR("\n WRITE_10 \n");
		//recieveDataFlag=true; // флаг о том что принимаем данные а не команду.
		//записываем в I начальный адрес записываемого блока
		i = ((cbw -> CBWCB[2] << 24) | (cbw -> CBWCB[3] << 16) | (cbw -> CBWCB[4] << 8) | (cbw -> CBWCB[5]));
		//записываем в n адрес последнего записываемого блока
		n = i + ((cbw -> CBWCB[7] << 8) | cbw -> CBWCB[8]);
		//выполняем чтение и запись блоков
		for ( ; i < n; i++)
		{
			//Так как размер конечной точки 64 байта, читаем 512 байт за 8 раз
			for (j = 0; j < 8; j++)
			{
				//TODO: реализовать чтение fifo по флагу
				//TODO: если FIFO заполнен
				//USB_DEVICE::read_BULK_FIFO(64);
				for(uint8_t j=0;j<64;j++)
				{
					buf[j+i]=USB_DEVICE::pThis->BULK_OUT_buf[j]; //заполнение массива 8 раз подряд
				}
				for(uint32_t i=0;i<300000;i++);//;ждем пока данные снова заполнят FIFO
			}
		//Записываем прочитанный блок во FLASH
			//Flash::pThis->write_any_buf(i, buf, 512);
		}
		//Заполняем и посылаем CSW
		CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength ) - cbw -> CBWCB[4];
		CSW.bCSWStatus = 0x00;
		USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
		break;
//-----------------------------------------------------------
		case TEST_UNIT_READY:
USART_debug::usart2_sendSTR("\n TEST_UNIT_READY \n");		
		CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength);
		CSW.bCSWStatus = 0x00;
		USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
		break;
		//Неизвестная команда
        default:
		//Заполняем поля CSW
            CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength);
		//Сообщаем об ошибке выполнения команды
            CSW.bCSWStatus = 0x01;
		//Посылаем контейнер состояния
            USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
		//Подтверждаем
            CSW.bCSWStatus = 0x00;
		//Посылаем контейнер состояния
            USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
            break;
        }   
		recieveCommandFlag=false;
    }
}