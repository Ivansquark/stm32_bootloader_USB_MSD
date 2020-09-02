#include "scsi.hpp"

bool SCSI::recieveCommandFlag=false;
bool SCSI::transiveFifoFlag=false;
bool SCSI::bulkFifoFlag=false;

SCSI::SCSI(){}

/*! Ф-я обработки SCSI команд, должно быть прочитано FIFO и заполнена структура*/
void SCSI::SCSI_Execute(uint8_t ep_number)
{
	uint32_t i, n;
    //uint32_t status;
    uint8_t j;	
	/* приняли данные в EP1, Натягиваем scsi_cbw_t на приемный буфер*/
    scsi_cbw_t *cbw = (scsi_cbw_t *)USB_DEVICE::pThis->BULK_OUT_buf;
	//Если пакет успешно принят
	USB_DEVICE::pThis->resetFlag=cbw -> CBWCB[0];
	recieveCommandFlag=false;
	if (cbw->dCBWSignature==0x43425355)
	{
	//Сразу копируем значение dCBWTag в CSW.dCSWTag
        CSW.dCSWTag = (cbw -> dCBWTag);
		//USB_DEVICE::pThis->counter = cbw -> CBWCB[0];
	//Определяем пришедшую команду	
    	switch (cbw -> CBWCB[0])
		{
		/*!Если INQUIRY*/
    	case INQUIRY:
		//USART_debug::usart2_sendSTR("\n INQUIRY \n");		
		//Проверка битов EVPD и CMDDT
        	if (cbw -> CBWCB[1] == 0)
			{
				//Передаем стандартный ответ на INQUIRY				
				//USB_DEVICE::pThis->counter = USB_OTG_IN(1)->DTXFSTS; //размер оставшегося места в FIFO		
        	    USB_DEVICE::pThis->WriteINEP(ep_number, inquiry, cbw -> CBWCB[4]);				
				//USART_debug::usart2_send(cbw->CBWCB[4]); //размер
				//USART_debug::usart2_sendSTR(" standINQUIRY \n");
				//Заполняем поля CSW
        	    CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength) - cbw -> CBWCB[4];
				//Команда выполнилась успешно
        	    CSW.bCSWStatus = 0x00; 
				//Посылаем контейнер состояния
				while(transiveFifoFlag);
        	    USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13); //статус всегда ин пакет
				//USB_DEVICE::pThis->counter = (CSW);        
				USART_debug::usart2_sendSTR("stI\n");				
			} else 
			{
				USART_debug::usart2_sendSTR("errorI\n");
				//Заполняем поля CSW
        	    CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength);
				//Сообщаем об ошибке выполнения команды
        	    CSW.bCSWStatus = 0x01;
				//Посылаем контейнер состояния
				while(transiveFifoFlag);
        	    USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
				//Подтверждаем
        	    CSW.bCSWStatus = 0x00;
				//Посылаем контейнер состояния
				while(transiveFifoFlag);
        	    USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
        	}
    	break;
		/*!Если REQUEST_SENSE*/
		case REQUEST_SENSE:
		USART_debug::usart2_sendSTR("\n REQUEST_SENSE \n");
		//Отправляем пояснительные данные
		while(transiveFifoFlag);
    	USB_DEVICE::pThis->WriteINEP(ep_number, sense_data, 18);
		//Заполняем поля CSW
    	CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength) - cbw -> CBWCB[4];
		//Команда выполнилась успешно
    	CSW.bCSWStatus = 0x00;
		//Посылаем контейнер состояния
		while(transiveFifoFlag);
    	USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
    	break;
//---------------------------------------------------------------------------------		
		/*! Если READ_CAPACITY_10*/
		case READ_CAPACITY_10:	
		//Сбросить все TXFIFO
		//USB_OTG_FS->GRSTCTL = USB_OTG_GRSTCTL_TXFFLSH | USB_OTG_GRSTCTL_TXFNUM;
		//while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH); //очищаем Tx_FIFO, которое почему то переполняется	
		//Передаем структуру
		while(transiveFifoFlag);
		USB_DEVICE::pThis->WriteINEP(ep_number, capacity, 8);
		//Заполняем и передаем CSW
    	CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength) - cbw -> CBWCB[4];
    	CSW.bCSWStatus = 0x00;
		while(transiveFifoFlag);
    	USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
		USART_debug::usart2_sendSTR("\n READ_CAPACITY_10 \n");
    	break;
//--------------------------------------------------------------
		/*!--MODE_SENSE_6--*/
		case MODE_SENSE_6:
		USART_debug::usart2_sendSTR("\n MODE_SENSE_6 \n");
		while(transiveFifoFlag);
		USB_DEVICE::pThis->WriteINEP(ep_number, mode_sense_6, 4);
		CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength) - cbw -> CBWCB[4];
		CSW.bCSWStatus = 0x00;
		while(transiveFifoFlag);
		USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
		break;
//-----------------------------------------------------------------------------------		
		case READ_10:
		USART_debug::usart2_sendSTR("\n READ_10 \n");
		//записываем в I начальный адрес читаемого блока
		i = ((cbw -> CBWCB[2] << 24) | (cbw -> CBWCB[3] << 16) | (cbw -> CBWCB[4] << 8) | (cbw -> CBWCB[5]));
		//записываем в n адрес последнего читаемого блока
		n = i + ((cbw -> CBWCB[7] << 8) | cbw -> CBWCB[8]);
		//выполняем чтение и передачу блоков
		
		for ( ; i < n; i++)
		{
			//Читаем блок из FLASH, помещаем в массив uint8_t buf2K[2048]
			Flash::pThis->read_buf(Flash::FLASH_PROGRAMM_ADDRESS+i*2048, buf2K, 2048);
			//Так как размер конечной точки 64 байта, передаем 512 байт за 8 раз			
			for (j = 0; j < 32; j++)
			{			
				uint32_t count=0;	
				//Передаем часть буфера
				count++;
				while(transiveFifoFlag);
				USB_DEVICE::pThis->WriteINEP(ep_number, (buf2K+j*64), 64);
				//USART_debug::usart2_sendSTR("\n R_F \n");
				USB_DEVICE::pThis->counter = count;
				//while(transiveFifoFlag);
			}			
			//USB_DEVICE::pThis->TxFifoFlush();
		}
		//USB_DEVICE::pThis->WriteINEP(ep_number, (buf), 0);//ZLP  (не нужно)
		
		//Заполняем и посылаем CSW
		CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength) - cbw -> CBWCB[4];
		CSW.bCSWStatus = 0x00;
		USB_DEVICE::pThis->counter=USB_OTG_IN(2)->DTXFSTS;
		while(transiveFifoFlag);		
		for(uint32_t i=0;i<1000;i++);//pause to recieve ZLP
		USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
		USART_debug::usart2_sendSTR("Read_end \n");
		
		break;
//---------------------------------------------------------------------------------		
		case WRITE_10:
		
		//recieveDataFlag=true; // флаг о том что принимаем данные а не команду.
		//записываем в I начальный адрес записываемого блока
		i = ((cbw -> CBWCB[2] << 24) | (cbw -> CBWCB[3] << 16) | (cbw -> CBWCB[4] << 8) | (cbw -> CBWCB[5]));
		//записываем в n адрес последнего записываемого блока
		n = i + ((cbw -> CBWCB[7] << 8) | cbw -> CBWCB[8]);
		//for(uint32_t i=0;i<10000;i++);//;ждем пока данные снова заполнят FIFO
		USART_debug::usart2_sendSTR("\n WRITE_10 \n");
		//выполняем чтение и запись блоков
		for(uint32_t z=0;z<20000;z++);//;ждем пока данные заполнят FIFO несколько раз
				
		for(uint8_t j=0; j<n; j++) //делим по 2048 байт
		{			
			for(uint32_t i=0;i<32;i++)
			{
				if(SCSI::bulkFifoFlag)
				{
					//USB_DEVICE::pThis->read_BULK_FIFO(64);
					//SCSI::bulkFifoFlag=false;
				}
				//for(uint32_t z=0;z<20000;z++);//;ждем пока данные заполнят FIFO несколько раз
				for(uint8_t k=0;k<64;k++)
				{
					buf2K[64*i+k]=QueT<uint8_t,2048>::pThis->pop();//USB_DEVICE::pThis->BULK_OUT_buf[k]; //заполнение массива 8 раз подряд
				}
				//USART_debug::usart2_sendSTR("\n WR_64 \n");		
			}						
			Flash::pThis->write_any_buf(Flash::FLASH_PROGRAMM_ADDRESS+j*2048, buf2K, 2048); //записываем во флэш 2048 байт
		}
		
		//Заполняем и посылаем CSW
		CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength ) - cbw -> CBWCB[4];
		CSW.bCSWStatus = 0x00;
		while(transiveFifoFlag);
		USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
		USART_debug::usart2_sendSTR("\n WR_10 \n");
		break;
//----------------------------------------------------------
		case TEST_UNIT_READY:
		USART_debug::usart2_sendSTR("\n TEST_UNIT_READY \n");		
		CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength);
		CSW.bCSWStatus = 0x00;
		//while(transiveFifoFlag);
		USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
		break;
//-------------------------------------------------------------		
		case PREVENT_ALLOW_MEDIUM_REMOVAL:
		USART_debug::usart2_sendSTR("\n PREVENT_ALLOW_MEDIUM_REMOVAL \n");		
		CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength);
		CSW.bCSWStatus = 0x00;
		while(transiveFifoFlag);
		USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
		break;
//------------------------------------------------------------------------		
		//Неизвестная команда  отвечаем ошибкой
    	default:
		USART_debug::usart2_send(cbw -> CBWCB[0]);
		USART_debug::usart2_sendSTR("\n SCSI_ERR \n");		
		//Заполняем поля CSW
    	    CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength);
		//Сообщаем об ошибке выполнения команды
    	    CSW.bCSWStatus = 0x01;
		//Посылаем контейнер состояния
			while(transiveFifoFlag);
    	    USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
		//Подтверждаем
    	    CSW.bCSWStatus = 0x00;
		//Посылаем контейнер состояния
			while(transiveFifoFlag);
    	    USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
    	    break;
    	}   
	}       
}