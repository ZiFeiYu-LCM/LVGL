#include "i2c.h"
#include "stdio.h"


void IIC_Init(void){
	
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();   //使能GPIOB时钟

    //PH4,5初始化设置
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
		IIC_SCK(1);
		IIC_SDA(1);
}

void IIC_Start(void){
	IIC_SDA_OUT();
	IIC_SDA(1);
	IIC_SCK(1);
	delay_us(4);
	IIC_SDA(0);
	delay_us(4);
	IIC_SCK(0);
	delay_us(2);
}



void IIC_Stop(void){
	IIC_SCK(0);
	IIC_SDA(0);
	IIC_SDA_OUT();
	delay_us(4);
	IIC_SCK(1);
	delay_us(2);
	IIC_SDA(1);
	delay_us(4);
	
}

uint8_t IIC_Wait_Ack(void){
	uint8_t timeout = 0;
	IIC_SCK(0);
	
	IIC_SDA_IN();
	IIC_SCK(1);
	delay_us(2);
	
	while(IIC_Read_SDA()){
		timeout++;
		if(timeout>250){
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCK(0);
	return 0;
}



void IIC_Ack(void){
	IIC_SCK(0);
	IIC_SDA_OUT();
	IIC_SDA(0);
	delay_us(2);
	IIC_SCK(1);
	delay_us(2);
	IIC_SCK(0);
}

void IIC_NAck(void){
	IIC_SCK(0);
	IIC_SDA_OUT();
	delay_us(2);
	IIC_SDA(1);
	delay_us(2);
	IIC_SCK(1);
	delay_us(2);
	IIC_SCK(0);	
	
}


void IIC_Send_Byte(uint8_t data){
	IIC_SCK(0);
	IIC_SDA_OUT();
	for(uint8_t i=0;i<8;i++){
		IIC_SDA((data&0x80)>>7);
		data <<= 1;
		delay_us(2);
		IIC_SCK(1);
		delay_us(2);
		IIC_SCK(0);
		delay_us(2);
	}
}

//isack=1 发送ack   isack=0不发送
uint8_t IIC_Read_Byte(uint8_t isack){
	uint8_t data = 0x00;
	IIC_SCK(0);
	IIC_SDA_IN();
	for(uint8_t i=0;i<8;i++){
		IIC_SCK(1);
		delay_us(2);
		
		data = data << 1;
		if(IIC_Read_SDA())data++;

		IIC_SCK(0);
		delay_us(2);
	}
	
	if(isack)
		IIC_Ack();
	else
		IIC_NAck();
	
	return data;
}

