#include "touch.h"
#include "stdio.h"
#include "stdlib.h"
#include "i2c.h"

#define TP_READ_TIMES   5       /* 读取次数 */
#define TP_LOST_VAL     1       /* 丢弃值 */


_tp_dev tp_dev = {tp_init,TP_Scan,0,0,0,0,0,0};


/**
 * @brief       读取一个坐标值(x或者y)
 *   @note      连续读取TP_READ_TIMES次数据,对这些数据升序排列,
 *              然后去掉最低和最高TP_LOST_VAL个数, 取平均值
 *              设置时需满足: TP_READ_TIMES > 2*TP_LOST_VAL 的条件
 *
 * @param       cmd : 指令
 *   @arg       0XD0: 读取X轴坐标(@竖屏状态,横屏状态和Y对调.)
 *   @arg       0X90: 读取Y轴坐标(@竖屏状态,横屏状态和X对调.)
 *
 * @retval      读取到的数据(滤波后的), ADC值(12bit)
 */
void tp_init(void){
	IIC_Init();
}

uint16_t tp_read_xoy(uint8_t cmd)
{
    uint16_t i, j;
    uint16_t buf[TP_READ_TIMES];
    uint16_t sum = 0;
    uint16_t temp;

    for (i = 0; i < TP_READ_TIMES; i++)   /* 先读取TP_READ_TIMES次数据 */
    {
        buf[i] = lcd_spi_read_byte(cmd);
    }

    for (i = 0; i < TP_READ_TIMES - 1; i++)   /* 对数据进行排序 */
    {
        for (j = i + 1; j < TP_READ_TIMES; j++)
        {
            if (buf[i] > buf[j])   /* 升序排列 */
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }

    sum = 0;

    for (i = TP_LOST_VAL; i < TP_READ_TIMES - TP_LOST_VAL; i++)   /* 去掉两端的丢弃值 */
    {
        sum += buf[i];  /* 累加去掉丢弃值以后的数据. */
    }

    temp = sum / (TP_READ_TIMES - 2 * TP_LOST_VAL); /* 取平均值 */
    return temp;
}


/**
 * @brief       读取x, y坐标
 * @param       x,y: 读取到的坐标值  displaydir：屏幕方向   1是横屏  0是竖屏（默认）
 * @retval      无
 */
void tp_read_xy(uint16_t *x, uint16_t *y, uint8_t displaydir)
{
    uint16_t xval, yval;

    if (displaydir & 0X01)    /* X,Y方向与屏幕相反 */
    {
        xval = tp_read_xoy(0X90);   /* 读取X轴坐标AD值, 并进行方向变换 */
        yval = tp_read_xoy(0XD0);   /* 读取Y轴坐标AD值 */
    }
    else                            /* X,Y方向与屏幕相同 */
    {
        xval = tp_read_xoy(0XD0);   /* 读取X轴坐标AD值 */
        yval = tp_read_xoy(0X90);   /* 读取Y轴坐标AD值 */
    }

    *x = xval;
    *y = yval;
}




/* 连续两次读取X,Y坐标的数据误差最大允许值 */
#define TP_ERR_RANGE    50      /* 误差范围 */

/**
 * @brief       连续读取2次触摸IC数据, 并滤波
 *   @note      连续2次读取触摸屏IC,且这两次的偏差不能超过ERR_RANGE,满足
 *              条件,则认为读数正确,否则读数错误.该函数能大大提高准确度.
 *
 * @param       x,y: 读取到的坐标值
 * @retval      0, 失败; 1, 成功;
 */
uint8_t tp_read_xy2(uint16_t *x, uint16_t *y)
{
    uint16_t x1, y1;
    uint16_t x2, y2;

    tp_read_xy(&x1, &y1,0);   /* 读取第一次数据 */
    tp_read_xy(&x2, &y2,0);   /* 读取第二次数据 */
		
		

    /* 前后两次采样在+-TP_ERR_RANGE内 */
    if (((x2 <= x1 && x1 < x2 + TP_ERR_RANGE) || (x1 <= x2 && x2 < x1 + TP_ERR_RANGE)) &&
            ((y2 <= y1 && y1 < y2 + TP_ERR_RANGE) || (y1 <= y2 && y2 < y1 + TP_ERR_RANGE)))
    {
        *x = (x1 + x2) / 2;
        *y = (y1 + y2) / 2;
        return 1;
    }

    return 0;
}

void lcd_read_xy(uint16_t *x, uint16_t *y){
	tp_read_xy2(x,y);
	return;
}


void TP_Draw_Touch_Point(uint16_t x,uint16_t y,uint16_t color){
	LCD_DrawLine(x-12,y,x+13,y,color);
	LCD_DrawLine(x,y-12,x,y+12,color);	
	
	LCD_DrawPoint(x+1,y+1,color);
	LCD_DrawPoint(x-1,y-1,color);
	LCD_DrawPoint(x-1,y+1,color);
	LCD_DrawPoint(x+1,y-1,color);
	
	LCD_DrawCircle(x,y,6,color,1);
}



/*
校准程序，主题思想是：
通过点击四次校准点，来计算点击的校准点每两个点之间的距离与对称的两个点之间的距离是否在一个误差之内
不在误差之内则重新校准
*/
void TP_Adjust(void){
	uint16_t pos_tmp[4][2];//存放四次校准时  点击的坐标
	uint8_t cnt = 0;//第几次点击
	tp_dev.state = 0;//重置设备的点击状态
	
	//记录时间
	uint16_t outtime = 0;
	//重来存每两个点之间的距离
	uint32_t d1 = 0;
	uint32_t d2 = 0;
	//用来存放两个点形成的直角的两个边的距离，计算出的斜边则是两个点的距离
	//存放d1或d2中
	uint32_t tem1 = 0;
	uint32_t tem2 = 0;
	
	//误差
	float fac = 0;
	
	lcdDev.LCD_Clear();
	
	LCD_ShowStr(100,40,"TP Adjust",16,0);
	TP_Draw_Touch_Point(20,20,RED);//画第1个校准点
	
	while(1){
		TP_Scan(1);
		if((tp_dev.state&0xc0) == TP_CHAR_PRESS){
			outtime=0;
			tp_dev.state &= ~(1<<6);//按键置位
			
			pos_tmp[cnt][0] = tp_dev.x[0];
			pos_tmp[cnt][1] = tp_dev.y[0];
			cnt++;
			
			switch(cnt){
				case 1:
					TP_Draw_Touch_Point(20,20,WHITE);//清楚第1个校准点
					TP_Draw_Touch_Point(lcdDev.width-20,20,RED);//画第2个校准点
					break;
				case 2:
					TP_Draw_Touch_Point(lcdDev.width-20,20,WHITE);//清楚第2个校准点
					TP_Draw_Touch_Point(20,lcdDev.height-20,RED);//画第3个校准点
					break;
				case 3:
					TP_Draw_Touch_Point(20,lcdDev.height-20,WHITE);//清楚第3个校准点
					TP_Draw_Touch_Point(lcdDev.width-20,lcdDev.height-20,RED);//画第4个校准点
					break;
				case 4:{
					TP_Draw_Touch_Point(lcdDev.width-20,lcdDev.height-20,WHITE);//清楚第4个校准点
					
					
					//查看点击校准的四个点  竖边是否相等
					tem1=abs(pos_tmp[0][0]-pos_tmp[2][0]);//x1-x3
					tem2=abs(pos_tmp[0][1]-pos_tmp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,3的距离
					printf("1:  tem1=%d, tem2=%d\n",tem1,tem2);
					
					tem1=abs(pos_tmp[1][0]-pos_tmp[3][0]);//x2-x4
					tem2=abs(pos_tmp[1][1]-pos_tmp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,4的距离
					printf("2:  tem1=%d, tem2=%d\n",tem1,tem2);
					
					printf("第一个点：(%d,%d)，第二个点：(%d,%d)，第三个点：(%d,%d)，第四个点：(%d,%d)\n",pos_tmp[0][0],pos_tmp[0][1],pos_tmp[1][0],pos_tmp[1][1],pos_tmp[2][0],pos_tmp[2][1],pos_tmp[3][0],pos_tmp[3][1]);//显示数据   
					
					//理想状态下，d1和d2的大小应该相同，因此用d1/d2作为一个误差衡量
					fac=(float)d1/d2;	
					printf("d1=%d, d2=%d,  fac=%f\n",d1,d2,fac);
					if(fac<0.95||fac>1.05)//不合格
					{
						printf("1\n");
						cnt=0;
						lcdDev.LCD_Clear();
   	 				TP_Draw_Touch_Point(20,20,RED);								//画点第1个校准点
 						LCD_ShowStr(100,40,"failed to adjust!",16,1);
						continue;
					}
					
					//查看点击校准的四个点  横边是否相等
					tem1=abs(pos_tmp[0][0]-pos_tmp[1][0]);//x1-x3
					tem2=abs(pos_tmp[0][1]-pos_tmp[1][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,3的距离
					
					tem1=abs(pos_tmp[2][0]-pos_tmp[3][0]);//x2-x4
					tem2=abs(pos_tmp[2][1]-pos_tmp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						printf("2\n");
						cnt=0;
						lcdDev.LCD_Clear();
   	 				TP_Draw_Touch_Point(20,20,RED);								//画点1
 						LCD_ShowStr(100,40,"failed to adjust!",16,1);
						//TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
						continue;
					}				

					//查看点击校准的四个点  对角线是否相等
					tem1=abs(pos_tmp[1][0]-pos_tmp[2][0]);//x1-x3
					tem2=abs(pos_tmp[1][1]-pos_tmp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,4的距离
	
					tem1=abs(pos_tmp[0][0]-pos_tmp[3][0]);//x2-x4
					tem2=abs(pos_tmp[0][1]-pos_tmp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,3的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						printf("3\n");
						cnt=0;
						lcdDev.LCD_Clear();
   	 				TP_Draw_Touch_Point(20,20,RED);								//画点1
						LCD_ShowStr(100,40,"failed to adjust!",16,1);
 						//TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
						continue;
					}//正确了					
					
					tp_dev.xfac=(float)(lcdDev.width-40)/(pos_tmp[1][0]-pos_tmp[0][0]);//得到xfac		 
					tp_dev.xoff=(lcdDev.width-tp_dev.xfac*(pos_tmp[1][0]+pos_tmp[0][0]))/2;//得到xoff
						  
					tp_dev.yfac=(float)(lcdDev.height-40)/(pos_tmp[2][1]-pos_tmp[0][1]);//得到yfac
					tp_dev.yoff=(lcdDev.height-tp_dev.yfac*(pos_tmp[2][1]+pos_tmp[0][1]))/2;//得到yoff  
					if(abs((int)tp_dev.xfac)>2||abs((int)tp_dev.yfac)>2){
						//设备的横竖屏设置反了
						lcdDev.LCD_Clear();
						LCD_ShowStr(100,40,"The orientation of the screen is reversed!",16,1);
					}
					lcdDev.LCD_Clear();
					LCD_ShowStr(100,40,"adjust successful!",16,1);
					delay_ms(1000);
					lcdDev.LCD_Clear();
					return;
				}
			
			}
		}
		
		//十秒没有点击则自动推出
		delay_ms(10);
		outtime++;
		if(outtime>1000)
		{
			//TP_Get_Adjdata();
			break;
	 	}
	}
}

/*
mode :0 读取屏幕坐标    1 读取物理坐标
*/
uint8_t TP_Scan(uint8_t mode){
	if(T_INT==0){
		tp_read_xy2(&tp_dev.x[0],&tp_dev.y[0]);//默认读取的就是物理坐标
		if(mode==0){
			tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//将结果转换为屏幕坐标
			tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff;  
		}
		
		if((tp_dev.state&TP_PRESS_DOWN)==0){//之前没有按下屏幕
			
			tp_dev.state = TP_PRESS_DOWN | TP_CHAR_PRESS;
			tp_dev.x[4] = tp_dev.x[0];
			tp_dev.y[4] = tp_dev.y[0];
		}
	}else{
		if(tp_dev.state&TP_PRESS_DOWN){//之前有按下屏幕
			tp_dev.state &= ~(1<<7);
		}else{//之前没有按下屏幕
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		
		}	
	}
	return tp_dev.state&TP_PRESS_DOWN;
}

uint32_t FloatToUint32(float value) {
    union FloatToUint32 converter;
    converter.f = value;
    return converter.u;
}

float Uint32ToFloat(uint32_t value) {
    union FloatToUint32 converter;
    converter.u = value;
    return converter.f;
}


uint16_t ShortToUint16(float value) {
    union ShortToUint16 converter;
    converter.t = value;
    return converter.u;
}

short Uint16ToShort(uint16_t value) {
    union ShortToUint16 converter;
    converter.u = value;
    return converter.t;
}
