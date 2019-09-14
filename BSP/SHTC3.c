#include "SHTC3.h"
#include "I2C.h"
#include "USART.h"


uint16_t SHTC3_ID;
uint8_t SHTC3_ERR;//主要用来判断I2C写操作是否能收到从机的应答信号，如果没有收到应答信号说明异常，0：收到应答信号，非0：没有收到应答信号

uint16_t TEMP_DATA;
uint8_t HUM_DATA;

/*******************************************************************************
函数名称        ：SHTC3_Init
函数参数        ：void
函数返回值      ：void
函数说明        ：初始化I2C管脚
                  软件复位SHTC3 
                  读取SHTC3的ID值            
*******************************************************************************/
void SHTC3_Init(void)
{   
    I2C_Init();

    SHTC3_WAKE_UP();//为了防止仿真的时候SHTC3再SLEEP状态下复位运行时造成SHTC3无法响应主机发送的命令，由于在仿真的时候SHTC3芯片没有断电，还是处于休眠状态，SHTC3在休眠是无法响应除了唤醒命令的其它操作的
    
    SHTC3_WRITE_CMD(SHTC3_SOFT_RESET_CMD);//发送软件复位命令
    HAL_Delay(10);//延时等待复位稳定

    SHTC3_READ_ID(SHTC3_READ_ID_CMD);
	
	printf("\r\nSHTC3的ID值为：0x%04x\r\n",SHTC3_ID); 
}

/*******************************************************************************
函数名称        ：SHTC3_WRITE_CMD
函数参数        ：发送的命令值
函数返回值      ：void
函数说明        ：主机发送2个字节给SHTC3                
*******************************************************************************/
void SHTC3_WRITE_CMD(uint16_t cmd)
{    
    I2C_START();
    SHTC3_ERR = I2C_WRITE_BYTE(SHTC3_WIRTE_CMD);
    if(SHTC3_ERR != 0)
    {
        SHTC3_ERR = I2C_WRITE_BYTE((cmd>>8) & 0xFF);
        if(SHTC3_ERR != 0)
        {    
            SHTC3_ERR = I2C_WRITE_BYTE(cmd & 0xFF);    
            if(SHTC3_ERR != 0)
            {         
               I2C_STOP();
            }
        }
    }
}

/*******************************************************************************
函数名称        ：SHTC3_READ_ID
函数参数        ：void
函数返回值      ：void
函数说明        ：将ID值保存在全局变量 SHTC3_ID 中               
*******************************************************************************/
void SHTC3_READ_ID(uint16_t cmd)
{
    SHTC3_ID = 0;
           
    SHTC3_WRITE_CMD(SHTC3_READ_ID_CMD);             
    if(SHTC3_ERR != 0)
    {
        I2C_START();    
        SHTC3_ERR = I2C_WRITE_BYTE(SHTC3_READ_CMD);    
        if(SHTC3_ERR != 0)
        {
            SHTC3_ID = I2C_READ_BYTE(1);
            SHTC3_ID <<= 8;
            SHTC3_ID |= I2C_READ_BYTE(0);
            I2C_STOP();     
            
            SHTC3_ID &= 0x083F;       
        }
    }
}

/*******************************************************************************
函数名称        ：SHTC3_WAKE_UP
函数参数        ：void
函数返回值      ：void
函数说明        ：唤醒IC                
*******************************************************************************/
void SHTC3_WAKE_UP(void)
{
    SHTC3_WRITE_CMD(SHTC3_WAKE_UP_CMD);     
    
    if(SHTC3_ERR != 0)//没有收到响应信号，直接退出
    {
        I2C_STOP(); 
        //正常休眠唤醒之后需要等待一段稳定的时间发送命令，SHTC3才能响应主机
        I2C_DELAY(5000);//大约1ms            
    }
}

/*******************************************************************************
函数名称        ：SHTC3_INTO_SLEEP
函数参数        ：void
函数返回值      ：void
函数说明        ：进入休眠                
*******************************************************************************/
void SHTC3_INTO_SLEEP(void)
{
    SHTC3_WRITE_CMD(SHTC3_SLEEP_CMD);
    I2C_STOP();      
}

/*******************************************************************************
函数名称        ：SHTC3_MEASUREMENT_TEMP_HUM
函数参数        ：void
函数返回值      ：void
函数说明        ：SHTC3温湿度检测程序，检测步骤如下：
                   1、发送WAKE UP命令唤醒SHTC3芯片
                   2、发送测量温度湿度命令 总共有4个测量方式，可以查看规格书
                   3、等待测量完成
                   4、读取测量的数据，包括CRC校验码
                   5、发送SLEEP命令使IC进入休眠模式                 
*******************************************************************************/
void SHTC3_MEASUREMENT_TEMP_HUM(void)
{ 
	uint8_t Cnt; 
	uint8_t Temp_Check_Num;
	uint8_t Hum_Check_Num;
	
	uint16_t TEMP_MEASUREMENT;
	uint16_t HUM_MEASUREMENT; 

	uint32_t result;

	SHTC3_WAKE_UP();
    
    if(SHTC3_ERR != 0)
    {   
        SHTC3_WRITE_CMD(SHTC3_NOR_READ_TEMP_FIR_DIS_CMD);//SHTC3大约11ms~20ms检测一次温度    
        
        if(SHTC3_ERR != 0)
        {
            Cnt = 0;
            do//等待SHTC3检测完成，    
            {
                I2C_DELAY(5000);//大约1ms  
                I2C_START();  
                SHTC3_ERR = I2C_WRITE_BYTE(SHTC3_READ_CMD);
                Cnt++;
                if(Cnt == 25)//防止读不到响应时，程序卡死
                {
                    break;    
                } 
                        
            }while(SHTC3_ERR == 0);//正常读取到响应才退出
               
            if(SHTC3_ERR != 0)
            {
                //SHTC3检测完成，开始读取数据，连CRC总共6个byte    
                TEMP_MEASUREMENT = I2C_READ_BYTE(1);//温度数值高8位
                TEMP_MEASUREMENT <<= 8;
                TEMP_MEASUREMENT |= I2C_READ_BYTE(1);//温度数值低8位
                Temp_Check_Num = I2C_READ_BYTE(1);//温度CRC校验
    
                HUM_MEASUREMENT = I2C_READ_BYTE(1);
                HUM_MEASUREMENT <<= 8;
                HUM_MEASUREMENT |= I2C_READ_BYTE(1);
                Hum_Check_Num = I2C_READ_BYTE(0); 
                
                I2C_STOP();    
				
				
				Cnt = SHTC3_CRC_CHECK(TEMP_MEASUREMENT,Temp_Check_Num);
				if(Cnt == 0)
				{
					result = TEMP_MEASUREMENT*175*10;//保留一位小数点
					TEMP_DATA = (uint16_t)(result >> 16);
					
					if(TEMP_DATA >= 450)
					{
						//正温度
						TEMP_DATA -= 450;
						Cnt = TEMP_DATA%10;
						TEMP_DATA = TEMP_DATA/10;
						printf("采集到的温度度值是：%d.%d\r\n",TEMP_DATA,Cnt); 	
					}
					else
					{
						//负温度
						TEMP_DATA = 450 - TEMP_DATA;
						Cnt = TEMP_DATA%10;
						TEMP_DATA = TEMP_DATA/10;
						printf("采集到的温度度值是：-%d.%d\r\n",TEMP_DATA,Cnt); 							
						
					}	
				}
				
				Cnt = SHTC3_CRC_CHECK(HUM_MEASUREMENT,Hum_Check_Num);
				if(Cnt == 0)
				{
					result = HUM_MEASUREMENT*100;
					HUM_DATA = (uint8_t)(result >> 16);
					
					printf("采集到的湿度值是：%d%%\r\n",HUM_DATA);    	
				}
				printf("\r\n");
            }  
        }
        
        SHTC3_INTO_SLEEP();
    } 
}

/*******************************************************************************
函数名称        ：SHTC3_CRC_CHECK
函数参数        ：校验参数和校验码
函数返回值      ：0：校验正确   1：校验错误
函数说明        ：CRC校验：
                    CRC多项式为：x^8+x^5+x^4+1，即0x131
*******************************************************************************/
uint8_t SHTC3_CRC_CHECK(uint16_t DAT,uint8_t CRC_DAT)
{
    uint8_t i,t,temp;
    uint8_t CRC_BYTE;
   
    CRC_BYTE = 0xFF;  
    temp = (DAT>>8) & 0xFF; 
       
    for(t = 0;t < 2;t ++)
    {
        CRC_BYTE ^= temp;
        for(i = 0;i < 8;i ++)
        {
            if(CRC_BYTE & 0x80)
            {
                CRC_BYTE <<= 1;
                
                CRC_BYTE ^= 0x31;    
            }    
            else
            {
                CRC_BYTE <<= 1;    
            }
        }
        
        if(t == 0)
        {
            temp = DAT & 0xFF; 
        }
    }
    
    if(CRC_BYTE == CRC_DAT)
    {
        temp = 0;    
    }    
    else
    {
        temp = 1;    
    }
    
    return temp;
}

/*******************************************************************************
函数名称        ：SHTC3_TEST1
函数参数        ：void
函数返回值      ：void
函数说明        ：
*******************************************************************************/
void SHTC3_TEST1(void)
{   
    SHTC3_Init(); 
   
    while(1)
    {
        SHTC3_MEASUREMENT_TEMP_HUM();    
        HAL_Delay(1000);
    }   
}

/*******************************************************************************
函数名称        ：SHTC3_TEST1
函数参数        ：void
函数返回值      ：void
函数说明        ：测试进入休眠状态之后，发送除了WAKE UP命令，其他命令有没有响应
*******************************************************************************/
void SHTC3_TEST2(void)
{
    SHTC3_Init(); 
    SHTC3_MEASUREMENT_TEMP_HUM();
        
    //HAL_Delay(20);
    //SHTC3_WAKE_UP();
    while(1)
    {
        //SHTC3_READ_ID(SHTC3_READ_ID_CMD);//实测的时候用逻辑分析仪测量I2C信号，在休眠的时候只有写地址的时候有响应，发送读取ID时没有响应，在唤醒的状态下，写指令都有响应。
        
        //I2C_START();  
        //SHTC3_ERR = I2C_WRITE_BYTE(SHTC3_READ_CMD);//在休眠的时候也没有收到响应
        //I2C_STOP(); 
        
        //如果IC处于休眠的状态，SHTC只能识别写地址和WAKE UP命令，其他命令是不会反馈应答信号的
             
        HAL_Delay(500);
    }       
}


