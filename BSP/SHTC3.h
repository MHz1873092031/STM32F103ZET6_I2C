#ifndef __SHTC3_H
#define __SHTC3_H
#include "stm32f1xx_hal.h"



//SHTC3地址
#define SHTC3_ADDRESS                   0x70

//SHTC3 命令
#define SHTC3_WIRTE_CMD                 ((SHTC3_ADDRESS<<1)&0xFE)
#define SHTC3_READ_CMD                  ((SHTC3_ADDRESS<<1)|0x01)

#define	SHTC3_SOFT_RESET_CMD	        0x805D//软件复位命令
#define	SHTC3_READ_ID_CMD	            0xEFC8//读取SHTC3 ID命令
#define	SHTC3_WAKE_UP_CMD	            0x3517//将芯片从睡眠模式唤醒命令
#define	SHTC3_SLEEP_CMD	                0xB098//使芯片进入休眠命令

//Normal Mode Clock Stretching Enable Measurement Command
#define	SHTC3_NOR_READ_TEMP_FIR_EN_CMD	0x7CA2//
#define	SHTC3_NOR_READ_HUM_FIR_EN_CMD	0x5C24//

//Normal Mode Clock Stretching Disable Measurement Command
#define	SHTC3_NOR_READ_TEMP_FIR_DIS_CMD	0x7866//
#define	SHTC3_NOR_READ_HUM_FIR_DIS_CMD	0x58E0//

//Low Power Mode Clock Stretching Enable Measurement Command
#define	SHTC3_LOW_READ_TEMP_FIR_EN_CMD	0x6458//
#define	SHTC3_LOW_READ_HUM_FIR_EN_CMD	0x44DE//

//Low Power Mode Clock Stretching Disable Measurement Command
#define	SHTC3_LOW_READ_TEMP_FIR_DIS_CMD	0x609C//
#define	SHTC3_LOW_READ_HUM_FIR_DIS_CMD	0x401A//



//SHTC3 ID
#define _SHTC3_ID			            0x0807

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//函数声明
void SHTC3_Init(void);
void SHTC3_WRITE_CMD(uint16_t cmd);
void SHTC3_READ_ID(uint16_t cmd);
void SHTC3_WAKE_UP(void);
void SHTC3_INTO_SLEEP(void);
void SHTC3_MEASUREMENT_TEMP_HUM(void);
uint8_t SHTC3_CRC_CHECK(uint16_t DAT,uint8_t CRC_DAT);

void SHTC3_TEST1(void);
void SHTC3_TEST2(void);

#endif


