#include "eeprom.h"
#include "stdio.h"

void EEPROM_Init(void){
	IIC_Init();
}

void EEPROM_Write_Byte(uint16_t addr,uint8_t data){

	IIC_Start();
	if(EEPROM_TYPE > AT24C16){
		IIC_Send_Byte(EEPROM_WERITE_CMD);
		IIC_Wait_Ack();
		IIC_Send_Byte(addr >> 8);
	}else{
		IIC_Send_Byte(EEPROM_WERITE_CMD + ((addr/256)<<1));
	}
	
	IIC_Wait_Ack();
	IIC_Send_Byte(addr%256);
	IIC_Wait_Ack();
	IIC_Send_Byte(data);
	IIC_Wait_Ack();
	
	IIC_Stop();
	delay_ms(10);
}

uint8_t EEPROM_Read_Byte(uint16_t addr){
	uint8_t data;
	IIC_Start();
	if(EEPROM_TYPE > AT24C16){
		IIC_Send_Byte(EEPROM_WERITE_CMD);
		IIC_Wait_Ack();
		IIC_Send_Byte(addr >> 8);
	}else{
		IIC_Send_Byte(EEPROM_WERITE_CMD + ((addr/256)<<1));
	}
	IIC_Wait_Ack();
	IIC_Send_Byte(addr%256);
	IIC_Wait_Ack();
	
	
	IIC_Start();
	IIC_Send_Byte(EEPROM_READ_CMD);
	IIC_Wait_Ack();
	data=IIC_Read_Byte(0);
	IIC_Wait_Ack(); 
	IIC_Stop();
	return data;
}

uint8_t EEPROM_Check(void){
	printf("check\n");
	if(EEPROM_Read_Byte(255) == 0xAA){
		return 0;
	}else{
		EEPROM_Write_Byte(255,0xAA);
		if(EEPROM_Read_Byte(255) == 0xAA){
			return 0;
		}
	}
	printf("check over\n");
	return 1;
}

/*
len代表要写入的字节数  1   2   4
*/
void EEPROM_Write_Bytes(uint16_t addr,uint32_t data,uint8_t len){
	for(uint8_t i=0;i<len;i++){
		
		EEPROM_Write_Byte(addr+i, (data>>(8*(len-1-i)))&0x000000FF );
	
	}
}

uint32_t EEPROM_Read_Bytes(uint16_t addr,uint8_t len){
	uint32_t res;
	for(uint8_t i=0;i<len;i++){
		res <<= 8;
		res += EEPROM_Read_Byte(addr+i);
	}
	return res;
}





