#include "AT24CXX.h"
#include "I2C.h"
#include "USART.h"

uint8_t AT24CXX_ERR;//主要用来判断I2C写操作是否能收到从机的应答信号，如果没有收到应答信号说明异常，0：收到应答信号，非0：没有收到应答信号

uint8_t AT24C_TEMP;
uint8_t AT24C02_BUFF[256];

/*******************************************************************************
函数名称        ：AT24CXX_Init
函数参数        ：void
函数返回值      ：void
函数说明        ：初始化I2C管脚
                  软件复位SHTC3 
                  读取SHTC3的ID值            
*******************************************************************************/
void AT24CXX_Init(void)
{   
    I2C_Init();

}

/*******************************************************************************
函数名称        ：AT24CXX_WRITE_ONE_BYTE
函数参数        ：address:写入数据保存的地址，dat:要写入的数据
函数返回值      ：void
函数说明        ：将一个字节的数据写入到指定的AT24CXX存储地址                
*******************************************************************************/
void AT24CXX_WRITE_ONE_BYTE(uint16_t address,uint8_t dat)
{    
    I2C_START();
    AT24CXX_ERR = I2C_WRITE_BYTE(AT24CXX_WIRTE_CMD);
    if(AT24CXX_ERR != 0)//没有响应直接退出
    {
        AT24CXX_ERR = I2C_WRITE_BYTE(address & 0xFF);
        if(AT24CXX_ERR != 0)
        {    
            AT24CXX_ERR = I2C_WRITE_BYTE(dat);    
            if(AT24CXX_ERR != 0)
            {         
               I2C_STOP();
            }
        }
    }
}

/*******************************************************************************
函数名称        ：AT24CXX_READ_ONE_BYTE
函数参数        ：address:要读取到的数据地址
函数返回值      ：读取到的数据
函数说明        ：将一个字节的数据写入到指定的AT24CXX存储地址                
*******************************************************************************/
uint8_t AT24CXX_READ_ONE_BYTE(uint16_t address)
{    
    uint8_t dat;
    
    I2C_START();
    AT24CXX_ERR = I2C_WRITE_BYTE(AT24CXX_WIRTE_CMD);
    if(AT24CXX_ERR != 0)//没有响应直接退出
    {
        AT24CXX_ERR = I2C_WRITE_BYTE(address & 0xFF);
        if(AT24CXX_ERR != 0)
        {    
            I2C_START();
            
            AT24CXX_ERR = I2C_WRITE_BYTE(AT24CXX_READ_CMD);    
            if(AT24CXX_ERR != 0)
            {         
               dat = I2C_READ_BYTE(0); 
               I2C_STOP();
            }
        }
    }
    
    return dat;
}

/*******************************************************************************
函数名称        ：AT24CXX_READ_BUFF
函数参数        ：address:要读取数据的地址，*buffer:读取到的数据保存在buffer中，Len:要读取的数据长度
函数返回值      ：void
函数说明        ：连续读取存储IC内的数据                
*******************************************************************************/
void AT24CXX_READ_BUFF(uint16_t address,uint8_t *buffer,uint16_t Len)
{    
	uint16_t i;
    
    I2C_START();
    AT24CXX_ERR = I2C_WRITE_BYTE(AT24CXX_WIRTE_CMD);
    if(AT24CXX_ERR != 0)//没有响应直接退出
    {
        AT24CXX_ERR = I2C_WRITE_BYTE(address & 0xFF);
        if(AT24CXX_ERR != 0)
        {    
            I2C_START();
			AT24CXX_ERR = I2C_WRITE_BYTE(AT24CXX_READ_CMD);   
			if(AT24CXX_ERR != 0)
			{               
				for(i=0;i<Len;i++)
				{
					buffer[i] = I2C_READ_BYTE(0); 
				}
			}
			
			if(AT24CXX_ERR != 0)
			{         
				I2C_STOP();
			}			
        }
    }
}

/*******************************************************************************
函数名称        ：AT24CXX_WRITE_BUFF
函数参数        ：address:要写入数据的地址，*buffer:要写入的数据的首地址，Len:要写入的数据长度
函数返回值      ：void
函数说明        ：将buffer中的数据连续写入到指定的地址                
*******************************************************************************/
void AT24CXX_WRITE_BUFF(uint16_t address,uint8_t *Buffer,uint16_t Len)
{
	uint8_t i;
	uint16_t re_main;
	
	if(address >= 256)//对输入的地址进行限制，24C02只有256个字节的存储空间，其它型号的存储器IC可以通过查资料
	{
		return;
	}
	
	re_main = 256 - address;//计算出还有多少存储空间
	
	if(Len > re_main)//如果要写入的数据量超过剩余存储空间，则只写入剩余存储空间数量的数据
	{
		Len = re_main;
	}
	
	re_main = 16 - address%16;//计算当前页还可以写入多少个数据
	
	if(Len <= re_main)//如果要写入的数据小于等于当前页剩余的存储空间，则只写入Len个字节数据就好，不需要跨页操作
	{
		re_main = Len;
	}
	
	do
	{
		I2C_START();
		AT24CXX_ERR = I2C_WRITE_BYTE(AT24CXX_WIRTE_CMD);
		if(AT24CXX_ERR == 0)//没有响应直接退出
		{
			break;
		}
		
		I2C_WRITE_BYTE(address & 0xFF);
		for(i = 0;i < re_main;i ++)//最多连续写入一个页数据的大小
		{
			AT24CXX_ERR = I2C_WRITE_BYTE(Buffer[i]);
		}
		
		I2C_STOP();
		Wait_AT24CXX_WRITE_OK();//等待24C02完成擦写数据动作	
		
		if(re_main != Len)
		{
			address += re_main;//已经写入re_main个数据，
			Buffer += re_main;
			Len -= re_main;
			
			re_main = 16;//写一个页的的大小也是16个字节
			
			if(Len <= re_main)
			{
				re_main = Len;
			}	
		}
		else
		{
			break;//数据写入完成退出
		}
	}
	while(1);	
}


/*******************************************************************************
函数名称        ：AT24CXX_PRINTF
函数参数        ：void
函数返回值      ：void
函数说明        ：读取24C02中的数据，然后通过USART打印出来
*******************************************************************************/
void AT24CXX_PRINTF(void)
{
	uint16_t i;
	 
	printf("  \r\n");
	
	for(i = 0;i < 256;i ++)
	{	
		AT24C02_BUFF[i] = AT24CXX_READ_ONE_BYTE(i);
	}
	    
    HAL_Delay(20);
	
	for(i = 0;i < 256;i ++)
	{
		if((i%16) == 0)
		{
			printf("\r\n");
		}
		printf("%#X\t",AT24C02_BUFF[i]);		
	}
}
	
/*******************************************************************************
函数名称        ：Wait_AT24CXX_WRITE_OK
函数参数        ：void
函数返回值      ：void
函数说明        ：通过读取AT24CXX反馈的响应信号来判断是否可以发送下一个数据
*******************************************************************************/
void Wait_AT24CXX_WRITE_OK(void)
{
	uint8_t Wait_Cnt;

	Wait_Cnt = 50; 
	do
	{
		I2C_START();
		AT24CXX_ERR = I2C_WRITE_BYTE(AT24CXX_WIRTE_CMD);
		if(AT24CXX_ERR != 0)
		{
			I2C_STOP();//接收到响应信号退出
			break;
		}
			
	}while(Wait_Cnt--);

}

/*******************************************************************************
函数名称        ：AT24CXX_TEST1
函数参数        ：void
函数返回值      ：void
函数说明        ：
*******************************************************************************/
void AT24CXX_TEST1(void)
{   
	uint16_t i;
	
    AT24CXX_Init(); 
    
	for(i = 0;i < 256;i ++)
	{	
		AT24C02_BUFF[i] = 0xFF;
		AT24CXX_WRITE_ONE_BYTE(i,AT24C02_BUFF[i]);
		Wait_AT24CXX_WRITE_OK();//可以通过发送设备地址给从机，通过从机反馈的响应信号来判断从机是否可以正常通讯
	}
	
	AT24CXX_PRINTF();//打印读取24C02的数据


	for(i = 0;i < 256;i ++)
	{	
		AT24C02_BUFF[i] = i;
	}
	
	AT24CXX_WRITE_BUFF(10,AT24C02_BUFF,100);//在地址0x0A开始连续写入100个数据
	
	AT24CXX_PRINTF();
	  
}



