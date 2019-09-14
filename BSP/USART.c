
#include "USART.h"





UART_HandleTypeDef UART_Handler;

uint8_t aRxBuffer;


//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while(__HAL_UART_GET_FLAG(&UART_Handler,UART_FLAG_TC) == RESET){}; 
      
    UART_Handler.Instance->DR = (uint8_t) ch;
    
	return ch;
}



void USART_Init(void)
{
    UART_Handler.Instance = USART1;
    UART_Handler.Init.BaudRate = 9600;
    UART_Handler.Init.Mode = UART_MODE_TX_RX;
    UART_Handler.Init.StopBits = UART_STOPBITS_1;
    UART_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    UART_Handler.Init.Parity = UART_PARITY_NONE;
    UART_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    HAL_UART_Init(&UART_Handler);
    
    //HAL_UART_Receive_IT(&UART_Handler,aRxBuffer,2);
    __HAL_UART_ENABLE_IT(&UART_Handler, UART_IT_RXNE);
    
    HAL_NVIC_SetPriority(USART1_IRQn,2,1);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
             
}


void HAL_UART_MspInit(UART_HandleTypeDef *husart)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);    

}    

void USART1_IRQHandler(void)
{
    //HAL_UART_IRQHandler(&UART_Handler);
    
    if(__HAL_UART_GET_FLAG(&UART_Handler,UART_FLAG_RXNE))
    {
        aRxBuffer = UART_Handler.Instance->DR; 
        //HAL_UART_Transmit(&UART_Handler,&aRxBuffer,1,1000);
    }       
}









