#ifndef MYLCD_H
#define MYLCD_H
#include "main.h"
#include <math.h>

typedef struct
{
    volatile uint16_t LCD_REG;
    volatile uint16_t LCD_RAM;
} LCD_TypeDef;
#define LCD_BASE        ((uint32_t)(0x6C000000 | 0x000007FE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)



typedef void (*funcPtr_void)(void);
typedef void (*print_str)(uint16_t x,uint16_t y,char *str,uint8_t size,uint8_t mode);
	
typedef struct{
	//参数
	uint16_t id;
	uint16_t width;
	uint16_t height;
	uint16_t color;
	uint16_t back_color;
	uint8_t display_dir;
	
	uint16_t default_show_range[4];  //默认的gram显示区域  0:cs   1:ce   2:ps    4:pe
	
	//命令
	uint16_t xcmd;
	uint16_t ycmd;
	uint16_t gramxcmd;
	
	//函数
	print_str LCD_ShowStr;
	funcPtr_void LCD_Clear;
	funcPtr_void LCD_RsetShowRange;
	
}_lcdDev;
extern _lcdDev lcdDev;


extern uint32_t sysClockFreq;
extern double cyclesPer;


//画笔颜色
#define WHITE           0xFFFF
#define BLACK           0x0000
#define BLUE            0x001F
#define BRED            0XF81F
#define GRED            0XFFE0
#define GBLUE           0X07FF
#define RED             0xF800
#define MAGENTA         0xF81F
#define GREEN           0x07E0
#define CYAN            0x7FFF
#define YELLOW          0xFFE0
#define BROWN           0XBC40  //棕色
#define BRRED           0XFC07  //棕红色
#define GRAY            0X8430  //灰色

void LCD_init(void);
void LCD_initREG_5310(void);
void LCD_SetBackligh(uint8_t flag);

void LCD_W_REG_VAL(volatile uint16_t reg,volatile uint16_t data);
uint16_t LCD_R_REG_VAL(volatile uint16_t reg);
void LCD_W_REG(volatile uint16_t reg);
void LCD_W_DATA(volatile uint16_t data);
uint16_t LCD_R_DATA(void);

uint16_t LCD_id(void);
void LCD_clear(void);
//void LCD_ShowChar(void);
//void LCD_ShowStr();
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color);
void LCD_ShowChar(uint16_t x,uint16_t y,char c,uint8_t size,uint8_t mode);
void LCD_ShowStr(uint16_t x,uint16_t y,char *str,uint8_t size,uint8_t mode);
void LCD_FillRect(uint16_t x,uint16_t y, uint16_t width, uint16_t height,uint16_t color);
void LCD_SetShowRange(uint16_t cs,uint16_t ce,uint16_t ps,uint16_t pe);
void LCD_ResetGramRrange(void);
void LCD_DrawCircle(uint16_t xc,uint16_t yc,uint16_t r, uint16_t color, uint8_t mode);
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);


void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color);
void LCD_W_EEPROM(void);
//void LCD_W_EEPROM();
//void delay_init(void);
//void delay_ns(uint32_t ns);



#endif




