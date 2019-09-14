#include "SHTC3.h"
#include "I2C.h"
#include "USART.h"


uint16_t SHTC3_ID;
uint8_t SHTC3_ERR;//��Ҫ�����ж�I2Cд�����Ƿ����յ��ӻ���Ӧ���źţ����û���յ�Ӧ���ź�˵���쳣��0���յ�Ӧ���źţ���0��û���յ�Ӧ���ź�

uint16_t TEMP_DATA;
uint8_t HUM_DATA;

/*******************************************************************************
��������        ��SHTC3_Init
��������        ��void
��������ֵ      ��void
����˵��        ����ʼ��I2C�ܽ�
                  �����λSHTC3 
                  ��ȡSHTC3��IDֵ            
*******************************************************************************/
void SHTC3_Init(void)
{   
    I2C_Init();

    SHTC3_WAKE_UP();//Ϊ�˷�ֹ�����ʱ��SHTC3��SLEEP״̬�¸�λ����ʱ���SHTC3�޷���Ӧ�������͵���������ڷ����ʱ��SHTC3оƬû�жϵ磬���Ǵ�������״̬��SHTC3���������޷���Ӧ���˻������������������
    
    SHTC3_WRITE_CMD(SHTC3_SOFT_RESET_CMD);//���������λ����
    HAL_Delay(10);//��ʱ�ȴ���λ�ȶ�

    SHTC3_READ_ID(SHTC3_READ_ID_CMD);
	
	printf("\r\nSHTC3��IDֵΪ��0x%04x\r\n",SHTC3_ID); 
}

/*******************************************************************************
��������        ��SHTC3_WRITE_CMD
��������        �����͵�����ֵ
��������ֵ      ��void
����˵��        ����������2���ֽڸ�SHTC3                
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
��������        ��SHTC3_READ_ID
��������        ��void
��������ֵ      ��void
����˵��        ����IDֵ������ȫ�ֱ��� SHTC3_ID ��               
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
��������        ��SHTC3_WAKE_UP
��������        ��void
��������ֵ      ��void
����˵��        ������IC                
*******************************************************************************/
void SHTC3_WAKE_UP(void)
{
    SHTC3_WRITE_CMD(SHTC3_WAKE_UP_CMD);     
    
    if(SHTC3_ERR != 0)//û���յ���Ӧ�źţ�ֱ���˳�
    {
        I2C_STOP(); 
        //�������߻���֮����Ҫ�ȴ�һ���ȶ���ʱ�䷢�����SHTC3������Ӧ����
        I2C_DELAY(5000);//��Լ1ms            
    }
}

/*******************************************************************************
��������        ��SHTC3_INTO_SLEEP
��������        ��void
��������ֵ      ��void
����˵��        ����������                
*******************************************************************************/
void SHTC3_INTO_SLEEP(void)
{
    SHTC3_WRITE_CMD(SHTC3_SLEEP_CMD);
    I2C_STOP();      
}

/*******************************************************************************
��������        ��SHTC3_MEASUREMENT_TEMP_HUM
��������        ��void
��������ֵ      ��void
����˵��        ��SHTC3��ʪ�ȼ����򣬼�ⲽ�����£�
                   1������WAKE UP�����SHTC3оƬ
                   2�����Ͳ����¶�ʪ������ �ܹ���4��������ʽ�����Բ鿴�����
                   3���ȴ��������
                   4����ȡ���������ݣ�����CRCУ����
                   5������SLEEP����ʹIC��������ģʽ                 
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
        SHTC3_WRITE_CMD(SHTC3_NOR_READ_TEMP_FIR_DIS_CMD);//SHTC3��Լ11ms~20ms���һ���¶�    
        
        if(SHTC3_ERR != 0)
        {
            Cnt = 0;
            do//�ȴ�SHTC3�����ɣ�    
            {
                I2C_DELAY(5000);//��Լ1ms  
                I2C_START();  
                SHTC3_ERR = I2C_WRITE_BYTE(SHTC3_READ_CMD);
                Cnt++;
                if(Cnt == 25)//��ֹ��������Ӧʱ��������
                {
                    break;    
                } 
                        
            }while(SHTC3_ERR == 0);//������ȡ����Ӧ���˳�
               
            if(SHTC3_ERR != 0)
            {
                //SHTC3�����ɣ���ʼ��ȡ���ݣ���CRC�ܹ�6��byte    
                TEMP_MEASUREMENT = I2C_READ_BYTE(1);//�¶���ֵ��8λ
                TEMP_MEASUREMENT <<= 8;
                TEMP_MEASUREMENT |= I2C_READ_BYTE(1);//�¶���ֵ��8λ
                Temp_Check_Num = I2C_READ_BYTE(1);//�¶�CRCУ��
    
                HUM_MEASUREMENT = I2C_READ_BYTE(1);
                HUM_MEASUREMENT <<= 8;
                HUM_MEASUREMENT |= I2C_READ_BYTE(1);
                Hum_Check_Num = I2C_READ_BYTE(0); 
                
                I2C_STOP();    
				
				
				Cnt = SHTC3_CRC_CHECK(TEMP_MEASUREMENT,Temp_Check_Num);
				if(Cnt == 0)
				{
					result = TEMP_MEASUREMENT*175*10;//����һλС����
					TEMP_DATA = (uint16_t)(result >> 16);
					
					if(TEMP_DATA >= 450)
					{
						//���¶�
						TEMP_DATA -= 450;
						Cnt = TEMP_DATA%10;
						TEMP_DATA = TEMP_DATA/10;
						printf("�ɼ������¶ȶ�ֵ�ǣ�%d.%d\r\n",TEMP_DATA,Cnt); 	
					}
					else
					{
						//���¶�
						TEMP_DATA = 450 - TEMP_DATA;
						Cnt = TEMP_DATA%10;
						TEMP_DATA = TEMP_DATA/10;
						printf("�ɼ������¶ȶ�ֵ�ǣ�-%d.%d\r\n",TEMP_DATA,Cnt); 							
						
					}	
				}
				
				Cnt = SHTC3_CRC_CHECK(HUM_MEASUREMENT,Hum_Check_Num);
				if(Cnt == 0)
				{
					result = HUM_MEASUREMENT*100;
					HUM_DATA = (uint8_t)(result >> 16);
					
					printf("�ɼ�����ʪ��ֵ�ǣ�%d%%\r\n",HUM_DATA);    	
				}
				printf("\r\n");
            }  
        }
        
        SHTC3_INTO_SLEEP();
    } 
}

/*******************************************************************************
��������        ��SHTC3_CRC_CHECK
��������        ��У�������У����
��������ֵ      ��0��У����ȷ   1��У�����
����˵��        ��CRCУ�飺
                    CRC����ʽΪ��x^8+x^5+x^4+1����0x131
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
��������        ��SHTC3_TEST1
��������        ��void
��������ֵ      ��void
����˵��        ��
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
��������        ��SHTC3_TEST1
��������        ��void
��������ֵ      ��void
����˵��        �����Խ�������״̬֮�󣬷��ͳ���WAKE UP�������������û����Ӧ
*******************************************************************************/
void SHTC3_TEST2(void)
{
    SHTC3_Init(); 
    SHTC3_MEASUREMENT_TEMP_HUM();
        
    //HAL_Delay(20);
    //SHTC3_WAKE_UP();
    while(1)
    {
        //SHTC3_READ_ID(SHTC3_READ_ID_CMD);//ʵ���ʱ�����߼������ǲ���I2C�źţ������ߵ�ʱ��ֻ��д��ַ��ʱ������Ӧ�����Ͷ�ȡIDʱû����Ӧ���ڻ��ѵ�״̬�£�дָ�����Ӧ��
        
        //I2C_START();  
        //SHTC3_ERR = I2C_WRITE_BYTE(SHTC3_READ_CMD);//�����ߵ�ʱ��Ҳû���յ���Ӧ
        //I2C_STOP(); 
        
        //���IC�������ߵ�״̬��SHTCֻ��ʶ��д��ַ��WAKE UP������������ǲ��ᷴ��Ӧ���źŵ�
             
        HAL_Delay(500);
    }       
}


