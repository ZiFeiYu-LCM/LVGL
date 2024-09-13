#ifndef TOUCH_H
#define TOUCH_H
#include "main.h"
#include "lcdspi.h"
#include "mylcd.h"

#define TP_MAX_TOUCH 5

//这个表示屏幕被按下
#define TP_PRESS_DOWN 0x80
//这个表示屏幕之前没有被按下，这是第一次被按下
#define TP_CHAR_PRESS 0X40

typedef void (*void_void_fun)(void);
typedef uint8_t (*u8_u8_fun)(uint8_t mode);
typedef struct {

	void_void_fun init;
	u8_u8_fun scan;
	
	uint16_t x[TP_MAX_TOUCH];
	uint16_t y[TP_MAX_TOUCH];
	uint8_t state;
	//校准
	float xfac;
	float yfac;
	short xoff;
	short yoff;
	

}_tp_dev;


union FloatToUint32 {
    float f;
    uint32_t u;
};

union ShortToUint16 {
    short t;
    uint16_t u;
};



extern _tp_dev tp_dev;


void tp_init(void);
uint16_t tp_read_xoy(uint8_t cmd);
void tp_read_xy(uint16_t *x, uint16_t *y, uint8_t displaydir);
uint8_t tp_read_xy2(uint16_t *x, uint16_t *y);

void lcd_read_xy(uint16_t *x, uint16_t *y);	
uint8_t TP_Scan(uint8_t mode);


//校准
void TP_Draw_Touch_Point(uint16_t x,uint16_t y,uint16_t color);
void TP_Adjust(void);
uint8_t TP_Get_Adjust(void);


float Uint32ToFloat(uint32_t value);
uint32_t FloatToUint32(float value);
uint16_t ShortToUint16(float value);
short Uint16ToShort(uint16_t value);
#endif





