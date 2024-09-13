#ifndef EEPROM_H
#define EEPROM_H
#include "i2c.h"
#include "main.h"
#include "delay.h"

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	  8191
#define AT24C128	16383
#define AT24C256	32767  

//所使用的EEPROM类型
#define EEPROM_TYPE AT24C02

#define EEPROM_WERITE_CMD 0xA0
#define EEPROM_READ_CMD 0xA1


void EEPROM_Init(void);
void EEPROM_Write_Byte(uint16_t addr,uint8_t data);
uint8_t EEPROM_Read_Byte(uint16_t addr);
uint8_t EEPROM_Check(void);



void EEPROM_Write_Bytes(uint16_t addr,uint32_t data,uint8_t len);
uint32_t EEPROM_Read_Bytes(uint16_t add,uint8_t len);


#endif



