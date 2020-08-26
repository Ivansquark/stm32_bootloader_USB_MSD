#ifndef SCSI_HPP
#define SCSI_HPP
#include "stdint.h"

const struct SCSI_NAME
{
    uint8_t TEST_UNIT_READY=0x01; 
    uint8_t REQUEST_SENSE = 0x03; 
    uint8_t INQUIRY = 0x12; 
    uint8_t MODE_SENSE_6 = 0x1A;                       
    uint8_t PREVENT_ALLOW_MEDIUM_REMOVAL  = 0x1E; 
    uint8_t READ_FORMAT_CAPACITIES = 0x23; 
    uint8_t READ_CAPACITY_10 = 0x25; 
    uint8_t READ_10 = 0x28;                        
    uint8_t WRITE_10 = 0x2A;
};
typedef struct cbw
{
    uint32_t dCBWSignature;
    uint32_t dCBWTag;
    uint32_t dCBWDataTransferLength;
    uint8_t bmCBWFlags;
    uint8_t bCBWLUN;
    uint8_t bCBWCBLength;
    uint8_t CBWCB[16];
}scsi_cbw;
typedef struct csw
{
    uint32_t dCSWSignature;
    uint32_t dCSWTag;
    uint32_t dCSWDataResidue;
    uint8_t bCSWStatus;
}scsi_csw;


#endif //SCSI_HPP
