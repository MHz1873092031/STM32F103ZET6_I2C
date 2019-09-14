#ifndef __AT24CXX_H
#define __AT24CXX_H
#include "stm32f1xx_hal.h"



//AT24CXXµØÖ·
#define AT24CXX_ADDRESS                   0xA0

//AT24CXX ÃüÁî
#define AT24CXX_WIRTE_CMD                 (AT24CXX_ADDRESS&0xFE)
#define AT24CXX_READ_CMD                  (AT24CXX_ADDRESS|0x01)


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//º¯ÊýÉùÃ÷
void AT24CXX_Init(void);
void AT24CXX_WRITE_ONE_BYTE(uint16_t address,uint8_t dat);
uint8_t AT24CXX_READ_ONE_BYTE(uint16_t address);


void AT24CXX_READ_BUFF(uint16_t address,uint8_t *buffer,uint16_t Len);
void AT24CXX_WRITE_BUFF(uint16_t address,uint8_t *Buffer,uint16_t Len);
void AT24CXX_PRINTF(void);
void Wait_AT24CXX_WRITE_OK(void);

void AT24CXX_TEST1(void);


#endif


