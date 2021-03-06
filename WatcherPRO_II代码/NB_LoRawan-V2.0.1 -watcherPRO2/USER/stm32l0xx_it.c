/**
  ******************************************************************************
  * @file    LCD/LCD_Blink_Frequency/Src/stm32l0xx_it.c 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    25-November-2016
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "stm32l0xx.h"
#include "stm32l0xx_it.h"
#include "rtc-board.h"
#include "timer-board.h"
#include "usart.h"

/** @addtogroup STM32L0xx_HAL_Examples
  * @{
  */

/** @addtogroup LCD_Blink_Frequency
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0+ Processor Exceptions Handlers                         */
/******************************************************************************/

extern uint8_t aRxBuffer5[RXBUFFERSIZE];

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
   DEBUG(2,"%s\r\n",__func__);
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

uint32_t TimerOverTime = 0; ///timer2超时机制

uint32_t Send_time;

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
	TimerOverTime ++;
	Send_time ++;
}


/******************************************************************************/
/* STM32L0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l0xx.s).                    */
/******************************************************************************/

extern bool test_rtc_state;


/**
* @brief This function handles RTC global interrupt through EXTI lines 17, 19 and 20 and LSE CSS interrupt through EXTI line 19.
*/
void RTC_IRQHandler(void)
{
  /* USER CODE BEGIN RTC_IRQn 0 */

  /* USER CODE END RTC_IRQn 0 */
	
  HAL_RTC_AlarmIRQHandler(&RtcHandle);
	
  DEBUG(3,"%s\r\n",__func__);
  /* USER CODE BEGIN RTC_IRQn 1 */
	
  test_rtc_state = false;


  /* USER CODE END RTC_IRQn 1 */
}

#define TX_TIME					   500000
#define TX_DUTYCYCLE_RND   		   100000

extern bool sendwkup;

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	DEBUG(3,"%s\r\n",__func__);
	
	if((__HAL_PWR_GET_FLAG( PWR_FLAG_WU ) != RESET) && LoRapp_SenSor_States.WKUP_State) 
	{
		/** disable irq */
		__disable_irq( );
		
		HAL_PWR_EnablePVD( );
		
		LoRapp_SenSor_States.WKUP_State = false;
		LoRaCad.cad_all_time = 0;
		sendwkup = true;
        
        ///串口不再printf
        huart1.gState = HAL_UART_STATE_RESET;  
        MX_USART1_UART_Init(  );	
        huart1.gState = HAL_UART_STATE_RESET; 
		BoardInitMcu( );
		
		HAL_RTC_DeactivateAlarm( &RtcHandle, RTC_ALARM_A );
		DEBUG(2,"wkup low-power\r\n");
		system_time = HAL_GetTick( );
		TimerStop( &ReportTimer );
		TimerSetValue( &ReportTimer, TX_TIME + randr( -TX_DUTYCYCLE_RND, TX_DUTYCYCLE_RND ) ); ///Get_Flash_Datas.sleep_times
		TimerStart( &ReportTimer );	
		
	}
	if( (__HAL_PWR_GET_FLAG( PWR_FLAG_WU ) != RESET)&& (LoRapp_SenSor_States.WKUP_State) ) 
	{
			__HAL_PWR_CLEAR_FLAG( PWR_FLAG_WU );
	}


	TimerIrqHandler( ); ///链表清零，重新计数	
 /** enable irq */
	__enable_irq( );
}

/**
* @brief This function handles USART4 and USART5 interrupt.
*/
void USART4_5_IRQHandler(void)
{
  /* USER CODE BEGIN USART4_5_IRQn 0 */

  /* USER CODE END USART4_5_IRQn 0 */
  HAL_UART_IRQHandler(&huart5);
  /* USER CODE BEGIN USART4_5_IRQn 1 */
	uint32_t timeout = 0;

	timeout = HAL_GetTick( );
	while (HAL_UART_GetState(&huart5)!=HAL_UART_STATE_READY)//等待就绪
	{
    if(HAL_GetTick( ) - timeout> 20) break;		///20ms超时处理
		printf("timeout--11 \r\n");
	}
     
	timeout = HAL_GetTick( );
	while(HAL_UART_Receive_IT(&huart5, UART_RX_DATA5.aRxBuffer, RXBUFFERSIZE)!=HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
    if(HAL_GetTick( ) - timeout> 20) break;	 	///20ms超时处理	
		printf("timeout \r\n");
	}
	DEBUG(3,"%s\r\n",__func__);

  /* USER CODE END USART4_5_IRQn 1 */
}


/**
* @brief This function handles USART2 global interrupt / USART2 wake-up interrupt through EXTI line 26.
*/
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */
	uint32_t timeout = 0;

	timeout = HAL_GetTick( );
	while (HAL_UART_GetState(&huart2)!=HAL_UART_STATE_READY)//等待就绪
	{
    if(HAL_GetTick( ) - timeout> 20) break;		///20ms超时处理
	}
     
	timeout = HAL_GetTick( );
	while(HAL_UART_Receive_IT(&huart2, UART_RX_DATA2.aRxBuffer, RXBUFFERSIZE)!=HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
    if(HAL_GetTick( ) - timeout> 20) break;	 	///20ms超时处理	
	}

  /* USER CODE END USART2_IRQn 1 */
}


/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
