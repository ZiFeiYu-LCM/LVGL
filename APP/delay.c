#include "delay.h"
#include <math.h>

/*
LOAD: ��ʾ�Ӷ��ٿ�ʼ��ʱ�ݼ���ÿ��һλʹ��һ��ʱ������

VAL: ��ʾ��LOAD�ݼ���ֵ

CTRL:
�� 0 λ (ENABLE): ���� SysTick������λΪ 1 ʱ��SysTick ��ʱ����ʼ�ݼ�������
�� 1 λ (TICKINT): ���������ݼ��� 0 ʱ�����жϡ�����λΪ 1 ʱ���жϻᱻʹ�ܡ�
�� 2 λ (CLKSOURCE): ѡ��ʱ��Դ��1 ��ʾʹ�ô�����ʱ�� (HCLK)��0 ��ʾʹ�� HCLK/8��
�� 16 λ (COUNTFLAG): ��λ�� SysTick �������ݼ��� 0 ʱ�ᱻӲ���� 1�������ֶ����ã���
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
	//SysTick->VAL=0X00;  ����SysTick->LOAD��ʱ��SysTick->VAL���Զ�����
	SysTick->CTRL=0x05; //ʹ��HCLK����ʼ��ʱ
	do{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&(!(temp&(1<<16))));  //�Ƿ��ʱ���
	SysTick->CTRL=0x00;   //�رռ�ʱ
  SysTick->VAL =0X00;
}

void delay_ms(uint16_t ms){
	uint32_t temp;
	SysTick->LOAD = ms*CLOCKFREQ*1000 - 1;
	//SysTick->VAL=0X00;  ����SysTick->LOAD��ʱ��SysTick->VAL���Զ�����
	SysTick->CTRL=0x05; //ʹ��HCLK����ʼ��ʱ
	do{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&(!(temp&(1<<16))));  //�Ƿ��ʱ���
	SysTick->CTRL=0x00;   //�رռ�ʱ
  SysTick->VAL =0X00;
}

void delay_ns1(uint16_t ns){//������ʱ����̵�nsʱ�䣬����72MHz��ÿ��ʱ��������13.8ns��  ���Ҽ�ʱ����15nsʱ���ʱ����ʱ�����ڣ������ڻ������Ҫ��ʱ��nsʱ��
	uint32_t temp;
	uint32_t conut = ceil(ns/CyclePer);//��Ҫ��ʱ��ʱ�� ���� һ�����ڵ�ʱ��  ������ȡ����������Ҫ����ʱ��������
	SysTick->LOAD = conut;
	SysTick->CTRL=0x05; //ʹ��HCLK����ʼ��ʱ  
	while((temp&0x01)&&(!(temp&(1<<16)))){//�Ƿ��ʱ���
		temp=SysTick->CTRL;
	}  
	SysTick->CTRL=0x00;   //�رռ�ʱ
  SysTick->VAL =0X00;
}



