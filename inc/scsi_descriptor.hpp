#ifndef SCSI_DESCRIPTOR_HPP
#define SCSI_DESCRIPTOR_HPP
#include "stdint.h"

static constexpr uint8_t inquiry[] = 
  {
    0x00,           //Block device
    0x80,           //Removable media
    0x04,           //SPC-2
    0x02,           //Response data format = 0x02
    0x1F,           //Additional_length = length - 5
    0x00,
    0x00,
    0x00,
    'I', 'V', 'A', 'N', ' ', 'O', 'p', 'A'
  };
static constexpr uint8_t sense_data[18] =
{
        0x70,       //VALID = 1, RESRONSE_CODE = 0x70
        0x00,
        0x05,       //S_ILLEGAL_REQUEST
        0x00, 
        0x00, 
        0x00, 
        0x00, 
        0x00,
        0x00, 
        0x00, 
        0x00,
        0x00, 
        0x00, 
        0x00, 
        0x00, 
        0x00, 
        0x00, 
        0x00
};

//----------------READ_FORMAT_CAPACITY----------------
//----------------READ_CAPACITY------------------------
static constexpr uint8_t format_capacity[12] = 
{
        0x00, 0x00, 0x00, 0x00,      
        0x00, 0x00, 0x00, 0x00,    
        0x02, 0x00, 0x00, 0x00
};

static constexpr uint8_t capacity[8] = 
{
        0x00, 0x00, 0x03, 0xFF,     //Addr last blocks = 4MB - 512B
        0x00, 0x00, 0x10, 0x00   
};
//---------------MODE_SENSE----------------------------
static constexpr uint8_t mode_sense_6[4] = 
{
        0x03, 0x00, 0x00, 0x00,
};



#endif //SCSI_DESCRIPTOR_HPP