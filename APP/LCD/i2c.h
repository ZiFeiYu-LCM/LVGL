#ifndef _MYIIC_H
#define _MYIIC_H

#include "main.h"
#include "delay.h"
#include "sys.h"

#define IIC_GPIO_SCK_PORT GPIOB
#define IIC_GPIO_SCK_PIN  GPIO_PIN_6

#define IIC_GPIO_SDA_PORT GPIOB
#define IIC_GPIO_SDA_PIN  GPIO_PIN_7

#define IIC_GPIO_SCK_ENABLE() do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0)
#define IIC_GPIO_SDA_ENABLE() do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0)

#define IIC_SDA_IN()	{GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
#define IIC_SDA_OUT()	{GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;} 



#define IIC_SCK(x) do{ x?\
											HAL_GPIO_WritePin(IIC_GPIO_SCK_PORT,IIC_GPIO_SCK_PIN,GPIO_PIN_SET):\
											HAL_GPIO_WritePin(IIC_GPIO_SCK_PORT,IIC_GPIO_SCK_PIN,GPIO_PIN_RESET);\
									 }while(0)

#define IIC_SDA(x) do{ x?\
											HAL_GPIO_WritePin(IIC_GPIO_SDA_PORT,IIC_GPIO_SDA_PIN,GPIO_PIN_SET):\
											HAL_GPIO_WritePin(IIC_GPIO_SDA_PORT,IIC_GPIO_SDA_PIN,GPIO_PIN_RESET);\
									 }while(0)


#define IIC_Read_SDA() HAL_GPIO_ReadPin(IIC_GPIO_SDA_PORT,IIC_GPIO_SDA_PIN)
							

void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t IIC_Wait_Ack(void);									 
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_Send_Byte(uint8_t data);
uint8_t IIC_Read_Byte(uint8_t isack);


void AT24CXX_WriteOneByte(uint8_t WriteAddr,uint8_t DataToWrite);
									 
#endif
									 
