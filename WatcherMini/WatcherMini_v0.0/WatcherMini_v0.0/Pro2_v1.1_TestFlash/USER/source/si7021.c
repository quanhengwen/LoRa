/*
**************************************************************************************************************
*	@file			SI7021.c
*	@author 	YSheng
*	@version 
*	@date    
*	@brief	����Ӳ��I2C����HAL_TIMEOUT��RCC_PERIPHCLK_I2C2ʱ��û��ʼ��, ����RCC_PERIPHCLK_I2C2ͬʱ���³�ʼ��I2C
***************************************************************************************************************
*/
#include <math.h>
#include "SI7021.h"

#define SI7021_WRITE_ADDR								0x80	// SI7021���豸��ַ: TI�ṩ7Bit ��ַ�����λBit ȱʡΪ0 ��1000000 =  0x80

/* Register addresses */
#define SI7021_Temperature 								0xE3	// Lux�Ĵ������ֽڵĵ�ַ
#define SI7021_Humidity					        		0xE5 //   

#define SI7021_Configuration            				0x3A

#define Firmware_ID_1									0x84
#define Firmware_ID_2 									0xB8

#define SI7021_EXP										16

#define version_1_0 									0xFF
#define version_2_0 									0x20

#define POLYNOMIAL 										0x31	//SI7021ʹ�õ�crc8У�����ʽ

uint8_t Firmware_ID[2] = {Firmware_ID_1,Firmware_ID_2};

// compound commands
uint8_t SERIAL1_READ[] = { 0xFA, 0x0F };
uint8_t SERIAL2_READ[] = { 0xFC, 0xC9 };

extern I2C_HandleTypeDef hi2c2;

/*
 *	InitHdc1080:		��ʼ��InitHdc1080��ַ
 *	����ֵ: 			
 */
void InitSI7021(void)
{
	//MX_I2C2_Init( );
}

/*
 *	SI7021ReadTH:	��ȡSI7021����ʪ��
 *	����ֵ: 			1�ɹ� 0ʧ��
 */
uint8_t SI7021ReadTH(int32_t *measure)
{
	float Temperature_Data = 0;
	float Humidity_Data = 0;
	uint8_t i, FID = 0;
	
	SI7021_Configtrue(SI7021_Configuration);  ///����SI7021_Configuration����ʼ��
	HAL_Delay(15);
    
	SI7021_WriteOneByte(SERIAL2_READ, 2); ///����CRC�ֽ�
	
	FID = SI7021_WriteOneByte(Firmware_ID, 2); ///��ȡ�汾ID
	HAL_Delay( 15 );
	
	if(FID == version_1_0 ||  FID == version_2_0) ///��ʼ�����
	{
		for(i=0;i<10;i++)
		{
			Temperature_Data += SI7021_ReadOneByte(SI7021_Temperature);
			HAL_Delay( 15 );	
	
			Humidity_Data += SI7021_ReadOneByte(SI7021_Humidity);
			HAL_Delay( 15 );	
		}			
        Temperature_Data = Temperature_Data/10;
        measure[0] = ((((175.72*Temperature_Data)/ exp2(SI7021_EXP))) - 46.85)*10;	
        
        Humidity_Data = Humidity_Data/10;
        measure[1] = ((125*Humidity_Data/ exp2(SI7021_EXP))-6)*10;
        printf("FID = %02x Temperature_Data = %d��C Humidity_Data = %d��RH\r\n", FID,measure[0],measure[1]);
        return 1;
	}
    return 0;
}

static void I2C_SI7021_Error (void)
{
  /* ����ʼ��I2Cͨ������ */
  HAL_I2C_DeInit(&hi2c2);
   
  /* ���³�ʼ��I2Cͨ������*/
  InitSI7021(  );
  printf("I2C TIMEOUT!!! I2C...\r\n");
}

//IICдһ���ֽ�  
//reg:�Ĵ�����ַ
//data:Ҫд�������
//����ֵ: ��
//    ����,�������
uint8_t SI7021_Configtrue(uint8_t reg)
{	
	HAL_StatusTypeDef state;
		
	state = HAL_I2C_Master_Transmit(&hi2c2,SI7021_WRITE_ADDR, &reg,1, 500);
	if(state!=HAL_OK)
	{
		printf("HAL_I2C_Master_Transmit state = %d\r\n",state);
		I2C_SI7021_Error(  );
		return 0;
	}
	return 1;

}

//IICдһ���ֽ�  
//reg:�Ĵ�����ַ
//data:Ҫд�������
//����ֵ: ��
//    ����,�������
uint8_t SI7021_WriteOneByte(uint8_t *data, uint8_t number)
{
	uint8_t Firmware;

	HAL_StatusTypeDef state;
	
	state = HAL_I2C_Master_Transmit(&hi2c2,SI7021_WRITE_ADDR, data, number, 500);
	if(state!=HAL_OK)
	{
		printf("HAL_I2C_Master_Transmit state = %d\r\n",state);
		I2C_SI7021_Error(  );
		return 0;
	}
	
	state = HAL_I2C_Master_Receive(&hi2c2,SI7021_WRITE_ADDR+1, &Firmware, 1, 500);
	if(state != HAL_OK)
	{
		printf("HAL_I2C_Master_Receive state222 = %d\r\n",state);
		I2C_SI7021_Error(  );
		return 0;
	}
	
	return Firmware;
}

//IIC��һ���ֽ� 
//reg:�Ĵ�����ַ 
//����ֵ:����������
uint16_t SI7021_ReadOneByte(uint8_t reg)
{
	uint8_t result[3] = {0};
	uint16_t Had_Data = 0;

	HAL_StatusTypeDef state;

	state = HAL_I2C_Master_Transmit(&hi2c2,SI7021_WRITE_ADDR, &reg, 1, 2000);
	if(state!=HAL_OK)
	{
		printf("HAL_I2C_Master_Transmit state = %d\r\n",state);
		I2C_SI7021_Error(  );
		return 0;
	}
	
	state = HAL_I2C_Master_Receive(&hi2c2,SI7021_WRITE_ADDR+1, result, 3, 2000);
	if(state != HAL_OK)
	{
		printf("HAL_I2C_Master_Receive state333 = %d\r\n",state);
		I2C_SI7021_Error(  );
		return 0;
	}	
	printf("high33 = %02x low33 = %02x Check_num = %02x\r\n", result[0],result[1], result[2]);
	
	if(SI7021_CheckCrc(result))
	{
		Had_Data = (result[0]<<8)|result[1];
	}else
	Had_Data = 0;
	
	return Had_Data;  
}

/*
 *	SI7021_CheckCrc:	��sht2x���������ݽ���У��
 *	measure��		�ɹ���1��ʧ�ܣ�0
 */
static uint8_t SI7021_CheckCrc(uint8_t data[])
{
   uint8_t crc = 0;
   uint8_t byteCtr;
   uint8_t bit;
   for (byteCtr = 0; byteCtr < 2; ++byteCtr)
    {
       crc ^= (data[byteCtr]);
       for (bit = 8; bit > 0; --bit)
       {
            if (crc & 0x80)
           	{
            	crc = (uint8_t)((crc<< 1) ^ (uint16_t)POLYNOMIAL);
           	}
            else
            {
             	crc = (crc<< 1);
            }
   		}
	}

	return (crc==data[2]);
}

