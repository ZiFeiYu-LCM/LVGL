#include "mylcd.h"
#include "font.h"
#include <stdio.h>
#include <stdlib.h>
uint32_t sysClockFreq = 72000000;   			//时钟频率
double cyclesPer = 1000000000/72000000.0; //每个始终中期用的时间
_lcdDev lcdDev;

void LCD_init(void){
	lcdDev.back_color = BLACK;
	lcdDev.color = RED;
	lcdDev.display_dir=0;
	lcdDev.id = LCD_id();
	lcdDev.LCD_ShowStr = LCD_ShowStr;
	lcdDev.LCD_Clear = LCD_clear;
	lcdDev.LCD_RsetShowRange = LCD_ResetGramRrange;
	if(lcdDev.id == 0x5310){
		lcdDev.width = 320;
		lcdDev.height = 480;
		
		lcdDev.gramxcmd = 0x2C;
		lcdDev.xcmd = 0x2A;
		lcdDev.ycmd = 0x2B;
		

		
		LCD_initREG_5310();
		
	}
	
	//设置默认gram区域
	lcdDev.default_show_range[0] = 0;
	lcdDev.default_show_range[1] = lcdDev.width-1;
	lcdDev.default_show_range[2] = 0;
	lcdDev.default_show_range[3] = lcdDev.height-1;
	LCD_SetBackligh(1);
}

/*
画线
(x1,y1)	开始坐标
(x2,y2)	结束坐标
color		颜色
*/
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color){
    int dx, dy, sx, sy, err, e2;
    
    dx = abs(x2 - x1);
    dy = abs(y2 - y1);
    sx = (x1 < x2) ? 1 : -1;
    sy = (y1 < y2) ? 1 : -1;
    err = dx - dy;
	
		while (1) {
				LCD_DrawPoint(x1,y1,color);
        // 检查是否已达到终点
        if (x1 == x2 && y1 == y2) break;
        
        e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
			}	
			
}

/*
画圆
(x,y)		圆心
r				半径
color		颜色
mode		是否实心圆  0 空心			1	实心
*/
void LCD_DrawCircle(uint16_t xc,uint16_t yc,uint16_t r, uint16_t color, uint8_t mode){
    int x = 0;
    int y = r;
    int d = 3 - 2 * r; // 初始决策参数
	
	
	
		if(mode == 0){
			// 在圆上绘制点，利用对称性
			while (x <= y) {			
				// 8个象限的对称点
				LCD_DrawPoint(xc + x, yc + y,color);
				LCD_DrawPoint(xc - x, yc + y,color);
				LCD_DrawPoint(xc + x, yc - y,color);
				LCD_DrawPoint(xc - x, yc - y,color);
				LCD_DrawPoint(xc + y, yc + x,color);
				LCD_DrawPoint(xc - y, yc + x,color);
				LCD_DrawPoint(xc + y, yc - x,color);
				LCD_DrawPoint(xc - y, yc - x,color);
						
				// 更新决策参数和点坐标
				if (d < 0) {
						d += 4 * x + 6;
				} else {
						d += 4 * (x - y) + 10;
						y--;
				}
				x++;
			}
		}else{
			while (x <= y) {
					// 绘制横向填充线
					for (int i = xc - x; i <= xc + x; i++) {
							LCD_DrawPoint(i, yc + y,color);
							LCD_DrawPoint(i, yc - y,color);
					}
					for (int i = xc - y; i <= xc + y; i++) {
							LCD_DrawPoint(i, yc + x,color);
							LCD_DrawPoint(i, yc - x,color);
					}

					// 更新决策参数和点坐标
					if (d < 0) {
							d += 4 * x + 6;
					} else {
							d += 4 * (x - y) + 10;
							y--;
					}
					x++;
			}
		}
}
// 填充圆的内部
void fill_circle(int xc, int yc, int r,uint16_t color) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r; // 初始决策参数
    
    while (x <= y) {
        // 绘制横向填充线
        for (int i = xc - x; i <= xc + x; i++) {
            LCD_DrawPoint(i, yc + y,color);
            LCD_DrawPoint(i, yc - y,color);
        }
        for (int i = xc - y; i <= xc + y; i++) {
            LCD_DrawPoint(i, yc + x,color);
            LCD_DrawPoint(i, yc - x,color);
        }

        // 更新决策参数和点坐标
        if (d < 0) {
            d += 4 * x + 6;
        } else {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}



void LCD_ResetGramRrange(void){
	LCD_SetShowRange(lcdDev.default_show_range[0],lcdDev.default_show_range[1],lcdDev.default_show_range[2],lcdDev.default_show_range[3]);
}

//设备背光，1为开启，0关闭
void LCD_SetBackligh(uint8_t flag){
	if(flag)HAL_GPIO_WritePin(LCD_BL_GPIO_Port,LCD_BL_Pin,GPIO_PIN_SET);
	else HAL_GPIO_WritePin(LCD_BL_GPIO_Port,LCD_BL_Pin,GPIO_PIN_RESET);
}
	

uint16_t LCD_id(void){
	LCD_W_REG(0XD4);
	LCD_R_DATA();
	LCD_R_DATA();
	uint16_t id;
	id = LCD_R_DATA();
	id = id << 8;
	id |= LCD_R_DATA();
	return id;
}

//写寄存器
void LCD_W_REG(volatile uint16_t reg){
	LCD->LCD_REG = reg;
}
//写寄存器值
void LCD_W_REG_VAL(volatile uint16_t reg,volatile uint16_t data){
	LCD_W_REG(reg);
	LCD_W_DATA(data);
}
//读寄存器值
uint16_t LCD_R_REG_VAL(volatile uint16_t reg){
	LCD_W_REG(reg);
	return LCD_R_DATA();
}
//写数据
void LCD_W_DATA(volatile uint16_t data){
	LCD->LCD_RAM = data;
}
//读数据
uint16_t LCD_R_DATA(void){
	volatile uint16_t data;
	data = LCD->LCD_RAM;
	return data;
}




void LCD_clear(void){
	LCD_W_REG(lcdDev.xcmd);
	LCD_W_DATA(0>>8);
	LCD_W_DATA(0&0XFF);
	LCD_W_DATA((lcdDev.width-1)>>8);
	LCD_W_DATA((lcdDev.width-1)&0XFF);
	LCD_W_REG(lcdDev.ycmd);
	LCD_W_DATA(0>>8);
	LCD_W_DATA(0&0XFF);
	LCD_W_DATA((lcdDev.height-1)>> 8);
	LCD_W_DATA((lcdDev.height-1)&0XFF);
	
	LCD_W_REG(lcdDev.gramxcmd);	
	uint32_t totalpoint = lcdDev.width*lcdDev.height;
	for (uint32_t index = 0; index < totalpoint; index++)
	{
			LCD_W_DATA(WHITE);
	}	
}




void LCD_SetPos(uint16_t x,uint16_t y){
	if(x>(lcdDev.width-1) || y>(lcdDev.height-1)) //判断坐标是否超出屏幕范围
		return;
	
	LCD_W_REG(lcdDev.xcmd);
	LCD_W_DATA(x >> 8);
	LCD_W_DATA(x & 0x00FF);
	LCD_W_REG(lcdDev.ycmd);
	LCD_W_DATA(y >> 8);
	LCD_W_DATA(y & 0x00FF);
}


void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color){
	LCD_SetPos(x,y);
	LCD_W_REG_VAL(lcdDev.gramxcmd,color);
}

/*
uint8_t x,uint8_t y  坐标
c 字符
size 字体大小  12/16/24
mode 字体是否自带背景 0自带  1不自带
*/
void LCD_ShowChar(uint16_t x,uint16_t y,char c,uint8_t size,uint8_t mode){
	uint16_t tmp_x = x, tmp_y = y;
	
	uint8_t csize = size/2 * ceil(size/8.0),tmp_c; 
	c -= ' ';
	
	for(int i=0;i<csize;i++){
		if(size == 12) tmp_c = asc2_1206[c][i];
		else if(size == 16) tmp_c = asc2_1608[c][i];
		else if(size == 24)	tmp_c = asc2_2412[c][i];
		else return;
		
		for(int j=0;j<8;j++){
			if(tmp_c & 0x80) LCD_DrawPoint(tmp_x, tmp_y, lcdDev.color);
			else if(mode==0) LCD_DrawPoint(tmp_x, tmp_y, lcdDev.back_color);
			tmp_c <<= 1;
			tmp_y++;
			if((tmp_y - y) >= size){
				tmp_y = y;
				tmp_x++;
			}
			if(tmp_y>=lcdDev.height || tmp_x>=lcdDev.width)//超出显示区域
				return;
		}
	}
}



void LCD_ShowStr(uint16_t x,uint16_t y,char *str,uint8_t size,uint8_t mode){
	uint16_t tmp_x = x, tmp_y = y;
	while ((*str <= '~') && (*str >= ' ')){
		
		LCD_ShowChar(tmp_x,tmp_y,*str,size,mode);
		tmp_x += (size/2);
		if(tmp_x >= lcdDev.width){
			tmp_x = x;
			tmp_y += size;
			if(tmp_y >= lcdDev.height)
				return;
		}
		str++;
	}
}









void LCD_SetShowRange(uint16_t cs,uint16_t ce,uint16_t ps,uint16_t pe){
	if(ce<=cs || pe<=ps)  //结束节点必须大于启始节点
		return;
	
	//范围超出的话就默认为最大范围
	if(ce >= lcdDev.width)
			ce = lcdDev.width-1;
	if(pe >= lcdDev.height)
			pe = lcdDev.height-1;
	
	LCD_W_REG(lcdDev.xcmd);
	LCD_W_DATA(cs>>8);
	LCD_W_DATA(cs&0XFF);
	LCD_W_DATA(ce>>8);
	LCD_W_DATA(ce&0XFF);
	LCD_W_REG(lcdDev.ycmd);
	LCD_W_DATA(ps>>8);
	LCD_W_DATA(ps&0XFF);
	LCD_W_DATA(pe>> 8);
	LCD_W_DATA(pe&0XFF);
}


//矩形填充
void LCD_FillRect(uint16_t x,uint16_t y, uint16_t width, uint16_t height,uint16_t color){
	if((x+width)>=lcdDev.width || (y+height)>=lcdDev.height)//超出屏幕范围
		return;
	
	LCD_SetShowRange(x,x+width,y,y+height);
	uint32_t total = width*height;
	LCD_W_REG(lcdDev.gramxcmd);
	for(int i=0;i<total ;i++){
		LCD_W_DATA(color);
	}
}



void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color){
	uint16_t height = ey-sy+1;
	uint16_t width = ex-sx+1;
	
	for(uint16_t i=0;i<height;i++){
		LCD_SetPos(sx,sy+i);
		LCD_W_REG(lcdDev.gramxcmd);
		for(uint16_t j=0;j<width;j++){
			LCD->LCD_RAM=color[i*width+j];
		}
	
	}
}





void LCD_initREG_5310(void){

        LCD_W_REG(0xED);
        LCD_W_DATA(0x01);
        LCD_W_DATA(0xFE);

        LCD_W_REG(0xEE);
        LCD_W_DATA(0xDE);
        LCD_W_DATA(0x21);

        LCD_W_REG(0xF1);
        LCD_W_DATA(0x01);
        LCD_W_REG(0xDF);
        LCD_W_DATA(0x10);

        //VCOMvoltage//
        LCD_W_REG(0xC4);
        LCD_W_DATA(0x8F);      //5f

        LCD_W_REG(0xC6);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xE2);
        LCD_W_DATA(0xE2);
        LCD_W_DATA(0xE2);
        LCD_W_REG(0xBF);
        LCD_W_DATA(0xAA);

        LCD_W_REG(0xB0);
        LCD_W_DATA(0x0D);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x0D);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x11);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x19);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x21);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x2D);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x3D);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x5D);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x5D);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xB1);
        LCD_W_DATA(0x80);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x8B);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x96);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xB2);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x02);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x03);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xB3);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xB4);
        LCD_W_DATA(0x8B);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x96);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xA1);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xB5);
        LCD_W_DATA(0x02);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x03);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x04);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xB6);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xB7);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x3F);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x5E);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x64);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x8C);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xAC);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xDC);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x70);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x90);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xEB);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xDC);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xB8);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xBA);
        LCD_W_DATA(0x24);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xC1);
        LCD_W_DATA(0x20);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x54);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xFF);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xC2);
        LCD_W_DATA(0x0A);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x04);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xC3);
        LCD_W_DATA(0x3C);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x3A);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x39);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x37);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x3C);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x36);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x32);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x2F);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x2C);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x29);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x26);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x24);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x24);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x23);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x3C);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x36);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x32);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x2F);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x2C);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x29);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x26);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x24);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x24);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x23);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xC4);
        LCD_W_DATA(0x62);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x05);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x84);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xF0);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x18);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xA4);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x18);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x50);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x0C);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x17);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x95);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xF3);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xE6);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xC5);
        LCD_W_DATA(0x32);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x44);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x65);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x76);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x88);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xC6);
        LCD_W_DATA(0x20);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x17);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x01);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xC7);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xC8);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xC9);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xE0);
        LCD_W_DATA(0x16);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x1C);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x21);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x36);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x46);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x52);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x64);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x7A);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x8B);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x99);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xA8);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xB9);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xC4);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xCA);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xD2);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xD9);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xE0);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xF3);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xE1);
        LCD_W_DATA(0x16);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x1C);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x22);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x36);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x45);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x52);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x64);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x7A);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x8B);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x99);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xA8);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xB9);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xC4);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xCA);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xD2);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xD8);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xE0);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xF3);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xE2);
        LCD_W_DATA(0x05);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x0B);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x1B);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x34);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x44);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x4F);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x61);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x79);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x88);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x97);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xA6);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xB7);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xC2);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xC7);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xD1);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xD6);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xDD);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xF3);
        LCD_W_DATA(0x00);
        LCD_W_REG(0xE3);
        LCD_W_DATA(0x05);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xA);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x1C);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x33);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x44);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x50);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x62);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x78);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x88);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x97);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xA6);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xB7);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xC2);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xC7);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xD1);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xD5);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xDD);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xF3);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xE4);
        LCD_W_DATA(0x01);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x01);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x02);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x2A);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x3C);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x4B);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x5D);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x74);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x84);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x93);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xA2);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xB3);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xBE);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xC4);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xCD);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xD3);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xDD);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xF3);
        LCD_W_DATA(0x00);
        LCD_W_REG(0xE5);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x02);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x29);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x3C);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x4B);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x5D);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x74);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x84);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x93);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xA2);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xB3);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xBE);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xC4);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xCD);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xD3);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xDC);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xF3);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xE6);
        LCD_W_DATA(0x11);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x34);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x56);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x76);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x77);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x66);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x88);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x99);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xBB);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x99);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x66);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x55);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x55);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x45);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x43);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x44);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xE7);
        LCD_W_DATA(0x32);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x55);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x76);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x66);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x67);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x67);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x87);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x99);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xBB);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x99);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x77);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x44);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x56);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x23);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x33);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x45);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xE8);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x99);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x87);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x88);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x77);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x66);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x88);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xAA);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0xBB);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x99);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x66);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x55);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x55);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x44);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x44);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x55);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xE9);
        LCD_W_DATA(0xAA);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);

        LCD_W_REG(0x00);
        LCD_W_DATA(0xAA);

        LCD_W_REG(0xCF);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xF0);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x50);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xF3);
        LCD_W_DATA(0x00);

        LCD_W_REG(0xF9);
        LCD_W_DATA(0x06);
        LCD_W_DATA(0x10);
        LCD_W_DATA(0x29);
        LCD_W_DATA(0x00);

        LCD_W_REG(0x3A);
        LCD_W_DATA(0x55);	//66

        LCD_W_REG(0x11);
        HAL_Delay(100);
        LCD_W_REG(0x29);
        LCD_W_REG(0x35);
        LCD_W_DATA(0x00);

        LCD_W_REG(0x51);
        LCD_W_DATA(0xFF);
        LCD_W_REG(0x53);
        LCD_W_DATA(0x2C);
        LCD_W_REG(0x55);
        LCD_W_DATA(0x82);
        LCD_W_REG(0x2c);

}





