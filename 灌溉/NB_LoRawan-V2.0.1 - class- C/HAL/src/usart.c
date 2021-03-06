/*
**************************************************************************************************************
*	@file	uart.c
*	@author Ysheng
*	@version 
*	@date    
*	@brief usart串口1，print串口测试doma
***************************************************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

#include "debug.h"
#include "gps.h"

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart5;

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
  
 /**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);  ///不需要库原因：从速度考虑直接使用寄存器更好
	return 1;
}

UART_RX UART_RX_DATA1 = {0, {0}, 0, {0}, false, false};
UART_RX UART_RX_DATA5 = {0, {0}, 0, {0}, false, false};

Set_Gps_Ack_t Set_Gps_Ack = {false, false, false, false, false, false, 0, 0, 0};



/* USART1 init function */

void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
	HAL_UART_Receive_IT(&huart1,UART_RX_DATA1.aRxBuffer, RXBUFFERSIZE);
}

void MX_USART5_UART_Init(void)
{
  huart5.Instance = USART5;
  huart5.Init.BaudRate = 9600;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
	
	HAL_UART_Receive_IT(&huart5, UART_RX_DATA5.aRxBuffer, RXBUFFERSIZE);
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  } else if(uartHandle->Instance==USART5)
  {
  /* USER CODE BEGIN USART5_MspInit 0 */

  /* USER CODE END USART5_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART5_CLK_ENABLE();
  
    /**USART5 GPIO Configuration    
    PB3     ------> USART5_TX
    PB4     ------> USART5_RX 
    */
    GPIO_InitStruct.Pin = USART5_TX|USART5_RX;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_USART5;
    HAL_GPIO_Init(USART5_IO, &GPIO_InitStruct);

  /* USER CODE BEGIN USART5_MspInit 1 */

  /* USER CODE END USART5_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(USART1_IRQn);

  }
  /* USER CODE BEGIN USART1_MspDeInit 1 */
	else if(uartHandle->Instance==USART5)
  {
  /* USER CODE BEGIN USART5_MspDeInit 0 */

  /* USER CODE END USART5_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART5_CLK_DISABLE();
  
    /**USART5 GPIO Configuration    
    PB3     ------> USART5_TX
    PB4     ------> USART5_RX 
    */
    HAL_GPIO_DeInit(USART5_IO, USART5_TX|USART5_RX);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(USART4_5_IRQn);

  /* USER CODE BEGIN USART5_MspDeInit 1 */

  /* USER CODE END USART5_MspDeInit 1 */
  }
  /* USER CODE END USART1_MspDeInit 1 */
} 

/* USER CODE BEGIN 1 */


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART5)//如果是串口5
	{				
		if(UART_RX_DATA5.aRxBuffer[0] == 0x0d)  ///以'\r''\n'作为结束标记
		{
			UART_RX_DATA5.Rx_State = true;
			DEBUG(2,"\n");		
		}else if(UART_RX_DATA5.aRxBuffer[0] == 0x0a) ///以'\n'作为结束标记
		{
			if(UART_RX_DATA5.Rx_State)
			{			
				UART_RX_DATA5.Rx_State = false;
				
				int8_t GLL_State = 0;
				
			 if(GLL_State == strcmp((char *)UART_RX_DATA5.USART_RX_BUF, "$PMTK010,002*2D"))
			 {
				 Set_Gps_Ack.START = true;
			 }
			 else if(GLL_State == strcmp((char *)UART_RX_DATA5.USART_RX_BUF, "$PMTK001,314,3*36"))
			 {
				 Set_Gps_Ack.GPGLL = true;
			 }
			 else if(GLL_State == strcmp((char *)UART_RX_DATA5.USART_RX_BUF, "$PMTK001,220,3*30"))
			 {
				 Set_Gps_Ack.POS_FIX = true;	
				 Set_Gps_Ack.GPS_OVER_TIME = HAL_GetTick( );
			 }

			 if(UART_RX_DATA5.USART_RX_BUF[UART_RX_DATA5.USART_RX_Len-6] == 'A')
			{
				DEBUG(3,"get poation\r\n");
				
				Set_Gps_Ack.GPS_COUNTER ++;
				
				if(Set_Gps_Ack.GPS_COUNTER>=20) ///定位20S后发送位置信息
				{
					Set_Gps_Ack.Get_PATION = true;
					Set_Gps_Ack.GPS_COUNTER = 0;
				}
				GPS_Disable(  );
					
				memcpy(Set_Gps_Ack.GLL, UART_RX_DATA1.USART_RX_BUF, UART_RX_DATA1.USART_RX_Len);
				Set_Gps_Ack.GLL[UART_RX_DATA5.USART_RX_Len++]='\r';
			    Set_Gps_Ack.GLL[UART_RX_DATA5.USART_RX_Len++]='\n';

				DEBUG(2,"GLL--%s",Set_Gps_Ack.GLL);

			}
			else if(UART_RX_DATA5.USART_RX_BUF[UART_RX_DATA5.USART_RX_Len-6] == 'V')
			{
				DEBUG(3,"get poation false\r\n"); 
				Set_Gps_Ack.Get_PATION = false;
				memcpy(Set_Gps_Ack.GLL, UART_RX_DATA5.USART_RX_BUF, UART_RX_DATA5.USART_RX_Len);
			}	
			DEBUG(3,"%s\r\n",UART_RX_DATA5.USART_RX_BUF); ///此处打印容易因为主频太低，打印丢包
			memset(UART_RX_DATA5.USART_RX_BUF, 0, UART_RX_DATA5.USART_RX_Len);
			UART_RX_DATA5.USART_RX_Len = 0;
		 }		
	 }
		else
		{
				UART_RX_DATA5.USART_RX_BUF[UART_RX_DATA5.USART_RX_Len]=UART_RX_DATA5.aRxBuffer[0];	
			  DEBUG(2,"%c",UART_RX_DATA5.USART_RX_BUF[UART_RX_DATA5.USART_RX_Len]);	
				UART_RX_DATA5.USART_RX_Len++;
		}	
		
	 if(UART_RX_DATA5.USART_RX_Len >= 516)
			UART_RX_DATA5.USART_RX_Len = 0;
	}	
}


/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
