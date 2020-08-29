#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H

constexpr uint8_t Device_Descriptor[18] =
    {
        /* Standard USB device descriptor for the CDC serial driver */
        0x12, // size
        0x01, // USBGenericDescriptor_DEVICE
        0x00,0x02, // USBDeviceDescriptor_USB 2_00
        0x00, // CDCDeviceDescriptor_CLASS  class defined at INTERFACE level
        0x00, // CDCDeviceDescriptor_SUBCLASS SubClass defined at INTERFACE level
        0x00, // CDCDeviceDescriptor_PROTOCOL - none
        0x40, // BOARD_USB_ENDPOINTS_MAXPACKETSIZE
        0x81,0x07, // CDCDSerialDriverDescriptors_VENDORID  Scandisk
        0x50,0x51, // CDCDSerialDriverDescriptors_PRODUCTID
        0x10,0x00, // CDCDSerialDriverDescriptors_RELEASE  bcdDevice (0010) 0.1
        1, // Index of manufacturer description //0
        2, // Index of product description //0
        3, // Index of serial number description //0
        1 // One possible configuration
    };
	/* configuration descriptor - 32 bytes*/
    constexpr struct
    {
        const uint8_t Config_Descriptor[9];
        const uint8_t Interface_Descriptor1[9];
        const uint8_t EP1_IN_Descriptor[7];
        const uint8_t EP1_OUT_Descriptor[7];
    }  confDescr =    
    {
        /*Configuration Descriptor*/
        {
            0x09, /* bLength: Configuration Descriptor size */
            0x02, /* bDescriptorType: Configuration */
            0x20,   /* wTotalLength:no of retuinturned bytes */ //32 байта
            0x00,
            0x01, /* bNumInterfaces: 2 interface */
            0x01, /* bConfigurationValue: Configuration value */
            0x00, /* iConfiguration: Index of string descriptor describing the configuration */
            0x80, /* bmAttributes - Bus powered 00 [6] 0-bus powered 1-no bus power [5] 0-no wakeup 1- wakeup */
            0x64 /* MaxPower 200 mA */
        },	
        /*Interface Descriptor MSC*/
        {
            0x09, /* bLength: Interface Descriptor size */
            0x04, /* bDescriptorType: Interface */
            0x00, /* bInterfaceNumber: Number of Interface */
            0x00, /* bAlternateSetting: Alternate setting */
            0x02, /* bNumEndpoints: TWO endpoints used */
            0x08, /* bInterfaceClass: Mass Storag Device Class */
            0x06, /* bInterfaceSubClass: SCSI transparent command set */
            0x50, /* bInterfaceProtocol: Bulk only transport */
            0x00 /* iInterface: */
        },
		{
			/*Endpoint 1 IN Descriptor*/
			0x07, /* bLength: Endpoint Descriptor size */
			0x05, /* bDescriptorType: Endpoint */
			0x82, /* bEndpointAddress IN2  8-IN 2-endpoint2*/
			0x02, /* bmAttributes: Data endpoint (D5-D4), No synchronization (D3-D2), Bulk transfer type (D1-D0) */
			64, /* wMaxPacketSize LO: */
			0x00, /* wMaxPacketSize HI: */
			0x00, /* bInterval: */
		},
		{
			/*Endpoint 1 OUT Descriptor*/
			0x07, /* bLength: Endpoint Descriptor size */
			0x05, /* bDescriptorType: Endpoint */
			0x02, /* bEndpointAddress OUT1  0-OUT 2-endpoint2*/
			0x02, /* bmAttributes: Data endpoint (D5-D4), No synchronization (D3-D2), Bulk transfer type (D1-D0) */
			64, /* wMaxPacketSize LO: */
			0x00, /* wMaxPacketSize HI: */
			0x01, /* bInterval: */
		}        
    };		
//---------------------------------------------------------------------------------------------------	
	/*! <Всего 67 байт>*/
	
	constexpr uint8_t LANG_ID_Descriptor[4]
    {
        /* Index 0x00: LANGID Codes */
        0x04,   /*descriptor length = 7 */
        0x03,   /*STRING descriptor type */
        0x09,	/*LANG_US uint16_t*/
		0x04      /*! !!! все что больше байта необходимо переворачивать*/  
    };   
	constexpr uint8_t Man_String[13]
	{
		/* Index 0x01:  */
		0x0a,   /*descriptor length = 13 */
        0x03,   /*STRING descriptor type */
        'I',0,
		'v',0,
		'a',0,
		'n',0
	};
	constexpr uint8_t Prod_String[14] 
	{
		/* Index 0x02:  */
		0x08,   /*descriptor length = 14 */
        0x03,   /*STRING descriptor type */
        'O',0,
		'p',0,
		'A',0		
	};
	constexpr uint8_t SN_String[14]
	{
		/* Index 0x03:  */
		0x08,   /*descriptor length = 14 */
        0x03,   /*STRING descriptor type */
        '1',0,
		'2',0,
		'3',0		
	};
	/*! <wRequest> */
	static constexpr uint16_t GET_STATUS_DEVICE = 0x8000; //вернуть два байта, во втором последние два бита [0]-sef power [1]-Remote Wakeup
	static constexpr uint16_t GET_STATUS_INTERFACE = 0x8100;
	static constexpr uint16_t GET_STATUS_ENDP = 0x8200;
    static constexpr uint16_t CLEAR_FEATURE_DEVICE = 0x0001;
	static constexpr uint16_t CLEAR_FEATURE_INTERFACE = 0x0101;
	static constexpr uint16_t CLEAR_FEATURE_ENDP = 0x0201;
    static constexpr uint16_t SET_FEATURE_DEVICE = 0x0003;
	static constexpr uint16_t SET_FEATURE_INTERFACE = 0x0103;
	static constexpr uint16_t SET_FEATURE_ENDP = 0x0203;
    static constexpr uint16_t SET_ADDRESS = 0x0005;
    static constexpr uint16_t GET_DESCRIPTOR_DEVICE = 0x8006;
	static constexpr uint16_t GET_DESCRIPTOR_INTERFACE = 0x8106;
	static constexpr uint16_t GET_DESCRIPTOR_ENDP = 0x8206;
    static constexpr uint16_t SET_DESCRIPTOR = 0x0007;
    static constexpr uint16_t GET_CONFIGURATION = 0x8008;
    static constexpr uint16_t SET_CONFIGURATION = 0x0009;
    static constexpr uint16_t GET_INTERFACE = 0x810A;
    static constexpr uint16_t SET_INTERFACE = 0x010B;
    static constexpr uint16_t SYNCH_FRAME = 0x820C;
	static constexpr uint16_t GET_REPORT = 0xA101;
	static constexpr uint16_t QUALIFIER = 0x0600;
	/*! <bmRequestType> */    
    static constexpr uint8_t STD_GET_STATUS = 0x00;
    static constexpr uint8_t STD_CLEAR_FEATURE = 0x01;
    static constexpr uint8_t STD_SET_FEATURE = 0x03;
    static constexpr uint8_t STD_SET_ADDRESS = 0x05;
    static constexpr uint8_t STD_GET_DESCRIPTOR = 0x06;
    static constexpr uint8_t STD_SET_DESCRIPTOR = 0x07;
    static constexpr uint8_t STD_GET_CONFIGURATION = 0x08;
	/*!<Драйвер в опреационной системе по окончанию энумерации посылает такой запрос>*/
    static constexpr uint8_t STD_SET_CONFIGURATION = 0x09; //в Value задается конфигурация в младшем байте
    static constexpr uint8_t STD_GET_INTERFACE = 0xA;
    static constexpr uint8_t STD_SET_INTERFACE = 0x11;
    static constexpr uint8_t STD_SYNCH_FRAME = 0x12;
    /*! <mValue> */
    static constexpr uint16_t USB_DESC_TYPE_DEVICE = 0x0100;
    static constexpr uint16_t USB_DESC_TYPE_CONFIGURATION = 0x0200;
    //static constexpr uint16_t USB_DESC_TYPE_DEVICE_QUALIFIER = 0x0600;
	
	/*<В запросах на передачу дескриптора Value содержит в старшем байте тип дескриптора, а в младшем индекс>*/
    static constexpr uint16_t USBD_IDX_LANGID_STR = 0x0300;
    static constexpr uint16_t USBD_strManufacturer = 0x0301;
    static constexpr uint16_t USBD_strProduct = 0x0302;
    static constexpr uint16_t USBD_IDX_SERIAL_STR = 0x0303;
    //static constexpr uint16_t USBD_IDX_CONFIG_STR = 0x0304;
	
	//<(bRequest<<8)|(bmRequestType)>
	
	/*!A DATA1 packet for GetMaxLun returns the number of LUNs (Logical Unit Numbers) supported by the device.
	If the device has 0 to 3 LUNs,it returns 3. If there is no LUN associated with,
	as in our case, it simply returns zero:*/
	static constexpr uint16_t GetMaxLun = 0xA1FE;  //FE   (если несколько носителей необходимо ответить сколько, если одно в ответе 0)
	static constexpr uint16_t BULK_ONLY_MSD_RESET = 0x21FF;
	
	/*!INQUIRY A DATA0 packet for Command Transport consists of 31 bytes of Command Block Wrapper data.*/
	/*! 3-0 	dCBWSignature (0x43425355) 
		7-4 	0x00000001
		11-8 	dCBWDataTransferLength 36 bytes, (0x00000024)
		12 		Device to host (0x80)
		13 		Reserved (0x0) bCBWLUN (0x0)
		14		Reserved (0x0) bCBWBLength 6 bytes (0x6)
		30-15	CBWCB The command block CBWCB has the INQUIRY command details.*/
		
	/*! 0 OPERATION CODE (0x12)
		1 Reserved (0x0) 0 0
		2 0x0
		3 Reserved (0x0)
		4 ALLOCATION LENGTH (0x24)
		5 CONTROL (0x00)*/		
	/* OUT-32 bytes (command transport) then IN 36-bytes (Data transport) then IN 13-bytes (Status)*/	
	//static constexpr uint16_t INQUIRY;
	//static constexpr uint16_t TEST_UNIT_READY = 
	//static constexpr uint16_t READ_CAPACITY =
	//static constexpr uint16_t MODE_SENSE =
	//static constexpr uint16_t REQUEST SENSE =
	//static constexpr uint16_t READ = 
	//static constexpr uint16_t WRITE = 
	
	static constexpr uint16_t SEND_ENCAPSULATED_COMMAND = 0x2100; //посылка команды
	static constexpr uint16_t GET_ENCAPSULATED_RESPONSE = 0xA101; // прием команды
	static constexpr uint16_t SET_LINE_CODING = 0x2120; //хост устанавливает параметры передачи данных
	static constexpr uint16_t GET_LINE_CODING = 0xA121;	// хост узнает текущие настройки линии передачи
	static constexpr uint16_t SET_CONTROL_LINE_STATE = 0x2122; // устанавливает состояние линии передачи (RTS[1] DTR[0])
	static constexpr uint16_t SEND_BREAK = 0x2123; //замораживает передачу данных
	
	constexpr uint8_t line_coding[7]={0x00,0xC2,0x01,0x00,0x00,0x00,0x08}; //uint32_t baudRate 115200; стоп бит, четность, размер посылки
	
	
#endif //USB_DESCRIPTORS_H