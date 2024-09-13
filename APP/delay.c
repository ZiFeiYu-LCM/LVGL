#include "delay.h"
#include <math.h>

/*
LOAD: 表示从多少开始计时递减，每减一位使用一个时钟周期

VAL: 表示从LOAD递减的值

CTRL:
第 0 位 (ENABLE): 启用 SysTick。当该位为 1 时，SysTick 计时器开始递减计数。
第 1 位 (TICKINT): 当计数器递减到 0 时产生中断。当该位为 1 时，中断会被使能。
第 2 位 (CLKSOURCE): 选择时钟源。1 表示使用处理器时钟 (HCLK)，0 表示使用 HCLK/8。
第 16 位 (COUNTFLAG): 该位在 SysTick 计数器递减到 0 时会被硬件置 1（不可手动设置）。
*/

uint8_t CLOCKFREQ =0;
double CyclePer = 0;


void delay_init(uint8_t clockfreq){
	CLOCKFREQ = clockfreq;
	CyclePer = 1.0/(CLOCKFREQ*1000000);
}


void delay_us(uint16_t us){
	uint32_t temp;
	SysTick->LOAD = us*CLOCKFREQ - 1;
	//SysTick->VAL=0X00;  设置SysTick->LOAD的时候，SysTick->VAL会自动清零
	SysTick->CTRL=0x05; //使用HCLK，开始计时
	do{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&(!(temp&(1<<16))));  //是否计时完成
	SysTick->CTRL=0x00;   //关闭计时
  SysTick->VAL =0X00;
}

void delay_ms(uint16_t ms){
	uint32_t temp;
	SysTick->LOAD = ms*CLOCKFREQ*1000 - 1;
	//SysTick->VAL=0X00;  设置SysTick->LOAD的时候，SysTick->VAL会自动清零
	SysTick->CTRL=0x05; //使用HCLK，开始计时
	do{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&(!(temp&(1<<16))));  //是否计时完成
	SysTick->CTRL=0x00;   //关闭计时
  SysTick->VAL =0X00;
}

void delay_ns1(uint16_t ns){//这个会计时出最短的ns时间，比如72MHz，每个时钟周期是13.8ns，  当我计时低于15ns时会计时两个时间周期，即等于或大于所要计时的ns时间
	uint32_t temp;
	uint32_t conut = ceil(ns/CyclePer);//所要计时的时间 除以 一个周期的时间  并向上取整，这是所要计算时的周期数
	SysTick->LOAD = conut;
	SysTick->CTRL=0x05; //使用HCLK，开始计时  
	while((temp&0x01)&&(!(temp&(1<<16)))){//是否计时完成
		temp=SysTick->CTRL;
	}  
	SysTick->CTRL=0x00;   //关闭计时
  SysTick->VAL =0X00;
}



