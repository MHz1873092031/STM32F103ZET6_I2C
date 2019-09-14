#ifndef __I2C_H
#define __I2C_H
#include "stm32f1xx_hal.h"




#define I2C_SCL(x)      (x ?  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET) )       
#define I2C_SDA(x)      (x ?  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET) )       
#define RD_I2C_SDA      HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)   

#define I2C_SDA_OUT()   {GPIOB->CRL &= 0x0FFFFFFF;GPIOB->CRL |= 0x30000000;} 
#define I2C_SDA_IN()    {GPIOB->CRL &= 0x0FFFFFFF;GPIOB->CRL |= 0x40000000;}
   

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//º¯ÊýÉùÃ÷
void I2C_Init(void);
void I2C_DELAY(uint16_t del);
void I2C_START(void);
void I2C_STOP(void);
uint8_t I2C_WAIT_ACK(void);
void I2C_ACK(void);
void I2C_NOT_ACK(void);
uint8_t I2C_WRITE_BYTE(uint8_t dat);
uint8_t I2C_READ_BYTE(uint8_t ack);

#endif


