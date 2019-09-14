#include "AT24CXX.h"
#include "I2C.h"
#include "USART.h"

uint8_t AT24CXX_ERR;//��Ҫ�����ж�I2Cд�����Ƿ����յ��ӻ���Ӧ���źţ����û���յ�Ӧ���ź�˵���쳣��0���յ�Ӧ���źţ���0��û���յ�Ӧ���ź�

uint8_t AT24C_TEMP;
uint8_t AT24C02_BUFF[256];

/*******************************************************************************
��������        ��AT24CXX_Init
��������        ��void
��������ֵ      ��void
����˵��        ����ʼ��I2C�ܽ�
                  �����λSHTC3 
                  ��ȡSHTC3��IDֵ            
*******************************************************************************/
void AT24CXX_Init(void)
{   
    I2C_Init();

}

/*******************************************************************************
��������        ��AT24CXX_WRITE_ONE_BYTE
��������        ��address:д�����ݱ���ĵ�ַ��dat:Ҫд�������
��������ֵ      ��void
����˵��        ����һ���ֽڵ�����д�뵽ָ����AT24CXX�洢��ַ                
*******************************************************************************/
void AT24CXX_WRITE_ONE_BYTE(uint16_t address,uint8_t dat)
{    
    I2C_START();
    AT24CXX_ERR = I2C_WRITE_BYTE(AT24CXX_WIRTE_CMD);
    if(AT24CXX_ERR != 0)//û����Ӧֱ���˳�
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
��������        ��AT24CXX_READ_ONE_BYTE
��������        ��address:Ҫ��ȡ�������ݵ�ַ
��������ֵ      ����ȡ��������
����˵��        ����һ���ֽڵ�����д�뵽ָ����AT24CXX�洢��ַ                
*******************************************************************************/
uint8_t AT24CXX_READ_ONE_BYTE(uint16_t address)
{    
    uint8_t dat;
    
    I2C_START();
    AT24CXX_ERR = I2C_WRITE_BYTE(AT24CXX_WIRTE_CMD);
    if(AT24CXX_ERR != 0)//û����Ӧֱ���˳�
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
��������        ��AT24CXX_READ_BUFF
��������        ��address:Ҫ��ȡ���ݵĵ�ַ��*buffer:��ȡ�������ݱ�����buffer�У�Len:Ҫ��ȡ�����ݳ���
��������ֵ      ��void
����˵��        ��������ȡ�洢IC�ڵ�����                
*******************************************************************************/
void AT24CXX_READ_BUFF(uint16_t address,uint8_t *buffer,uint16_t Len)
{    
	uint16_t i;
    
    I2C_START();
    AT24CXX_ERR = I2C_WRITE_BYTE(AT24CXX_WIRTE_CMD);
    if(AT24CXX_ERR != 0)//û����Ӧֱ���˳�
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
��������        ��AT24CXX_WRITE_BUFF
��������        ��address:Ҫд�����ݵĵ�ַ��*buffer:Ҫд������ݵ��׵�ַ��Len:Ҫд������ݳ���
��������ֵ      ��void
����˵��        ����buffer�е���������д�뵽ָ���ĵ�ַ                
*******************************************************************************/
void AT24CXX_WRITE_BUFF(uint16_t address,uint8_t *Buffer,uint16_t Len)
{
	uint8_t i;
	uint16_t re_main;
	
	if(address >= 256)//������ĵ�ַ�������ƣ�24C02ֻ��256���ֽڵĴ洢�ռ䣬�����ͺŵĴ洢��IC����ͨ��������
	{
		return;
	}
	
	re_main = 256 - address;//��������ж��ٴ洢�ռ�
	
	if(Len > re_main)//���Ҫд�������������ʣ��洢�ռ䣬��ֻд��ʣ��洢�ռ�����������
	{
		Len = re_main;
	}
	
	re_main = 16 - address%16;//���㵱ǰҳ������д����ٸ�����
	
	if(Len <= re_main)//���Ҫд�������С�ڵ��ڵ�ǰҳʣ��Ĵ洢�ռ䣬��ֻд��Len���ֽ����ݾͺã�����Ҫ��ҳ����
	{
		re_main = Len;
	}
	
	do
	{
		I2C_START();
		AT24CXX_ERR = I2C_WRITE_BYTE(AT24CXX_WIRTE_CMD);
		if(AT24CXX_ERR == 0)//û����Ӧֱ���˳�
		{
			break;
		}
		
		I2C_WRITE_BYTE(address & 0xFF);
		for(i = 0;i < re_main;i ++)//�������д��һ��ҳ���ݵĴ�С
		{
			AT24CXX_ERR = I2C_WRITE_BYTE(Buffer[i]);
		}
		
		I2C_STOP();
		Wait_AT24CXX_WRITE_OK();//�ȴ�24C02��ɲ�д���ݶ���	
		
		if(re_main != Len)
		{
			address += re_main;//�Ѿ�д��re_main�����ݣ�
			Buffer += re_main;
			Len -= re_main;
			
			re_main = 16;//дһ��ҳ�ĵĴ�СҲ��16���ֽ�
			
			if(Len <= re_main)
			{
				re_main = Len;
			}	
		}
		else
		{
			break;//����д������˳�
		}
	}
	while(1);	
}


/*******************************************************************************
��������        ��AT24CXX_PRINTF
��������        ��void
��������ֵ      ��void
����˵��        ����ȡ24C02�е����ݣ�Ȼ��ͨ��USART��ӡ����
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
��������        ��Wait_AT24CXX_WRITE_OK
��������        ��void
��������ֵ      ��void
����˵��        ��ͨ����ȡAT24CXX��������Ӧ�ź����ж��Ƿ���Է�����һ������
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
			I2C_STOP();//���յ���Ӧ�ź��˳�
			break;
		}
			
	}while(Wait_Cnt--);

}

/*******************************************************************************
��������        ��AT24CXX_TEST1
��������        ��void
��������ֵ      ��void
����˵��        ��
*******************************************************************************/
void AT24CXX_TEST1(void)
{   
	uint16_t i;
	
    AT24CXX_Init(); 
    
	for(i = 0;i < 256;i ++)
	{	
		AT24C02_BUFF[i] = 0xFF;
		AT24CXX_WRITE_ONE_BYTE(i,AT24C02_BUFF[i]);
		Wait_AT24CXX_WRITE_OK();//����ͨ�������豸��ַ���ӻ���ͨ���ӻ���������Ӧ�ź����жϴӻ��Ƿ��������ͨѶ
	}
	
	AT24CXX_PRINTF();//��ӡ��ȡ24C02������


	for(i = 0;i < 256;i ++)
	{	
		AT24C02_BUFF[i] = i;
	}
	
	AT24CXX_WRITE_BUFF(10,AT24C02_BUFF,100);//�ڵ�ַ0x0A��ʼ����д��100������
	
	AT24CXX_PRINTF();
	  
}



