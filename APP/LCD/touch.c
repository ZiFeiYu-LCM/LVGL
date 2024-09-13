#include "touch.h"
#include "stdio.h"
#include "stdlib.h"
#include "i2c.h"

#define TP_READ_TIMES   5       /* ��ȡ���� */
#define TP_LOST_VAL     1       /* ����ֵ */


_tp_dev tp_dev = {tp_init,TP_Scan,0,0,0,0,0,0};


/**
 * @brief       ��ȡһ������ֵ(x����y)
 *   @note      ������ȡTP_READ_TIMES������,����Щ������������,
 *              Ȼ��ȥ����ͺ����TP_LOST_VAL����, ȡƽ��ֵ
 *              ����ʱ������: TP_READ_TIMES > 2*TP_LOST_VAL ������
 *
 * @param       cmd : ָ��
 *   @arg       0XD0: ��ȡX������(@����״̬,����״̬��Y�Ե�.)
 *   @arg       0X90: ��ȡY������(@����״̬,����״̬��X�Ե�.)
 *
 * @retval      ��ȡ��������(�˲����), ADCֵ(12bit)
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

    for (i = 0; i < TP_READ_TIMES; i++)   /* �ȶ�ȡTP_READ_TIMES������ */
    {
        buf[i] = lcd_spi_read_byte(cmd);
    }

    for (i = 0; i < TP_READ_TIMES - 1; i++)   /* �����ݽ������� */
    {
        for (j = i + 1; j < TP_READ_TIMES; j++)
        {
            if (buf[i] > buf[j])   /* �������� */
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }

    sum = 0;

    for (i = TP_LOST_VAL; i < TP_READ_TIMES - TP_LOST_VAL; i++)   /* ȥ�����˵Ķ���ֵ */
    {
        sum += buf[i];  /* �ۼ�ȥ������ֵ�Ժ������. */
    }

    temp = sum / (TP_READ_TIMES - 2 * TP_LOST_VAL); /* ȡƽ��ֵ */
    return temp;
}


/**
 * @brief       ��ȡx, y����
 * @param       x,y: ��ȡ��������ֵ  displaydir����Ļ����   1�Ǻ���  0��������Ĭ�ϣ�
 * @retval      ��
 */
void tp_read_xy(uint16_t *x, uint16_t *y, uint8_t displaydir)
{
    uint16_t xval, yval;

    if (displaydir & 0X01)    /* X,Y��������Ļ�෴ */
    {
        xval = tp_read_xoy(0X90);   /* ��ȡX������ADֵ, �����з���任 */
        yval = tp_read_xoy(0XD0);   /* ��ȡY������ADֵ */
    }
    else                            /* X,Y��������Ļ��ͬ */
    {
        xval = tp_read_xoy(0XD0);   /* ��ȡX������ADֵ */
        yval = tp_read_xoy(0X90);   /* ��ȡY������ADֵ */
    }

    *x = xval;
    *y = yval;
}




/* �������ζ�ȡX,Y�������������������ֵ */
#define TP_ERR_RANGE    50      /* ��Χ */

/**
 * @brief       ������ȡ2�δ���IC����, ���˲�
 *   @note      ����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���ERR_RANGE,����
 *              ����,����Ϊ������ȷ,�����������.�ú����ܴ�����׼ȷ��.
 *
 * @param       x,y: ��ȡ��������ֵ
 * @retval      0, ʧ��; 1, �ɹ�;
 */
uint8_t tp_read_xy2(uint16_t *x, uint16_t *y)
{
    uint16_t x1, y1;
    uint16_t x2, y2;

    tp_read_xy(&x1, &y1,0);   /* ��ȡ��һ������ */
    tp_read_xy(&x2, &y2,0);   /* ��ȡ�ڶ������� */
		
		

    /* ǰ�����β�����+-TP_ERR_RANGE�� */
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
У׼��������˼���ǣ�
ͨ������Ĵ�У׼�㣬����������У׼��ÿ������֮��ľ�����ԳƵ�������֮��ľ����Ƿ���һ�����֮��
�������֮��������У׼
*/
void TP_Adjust(void){
	uint16_t pos_tmp[4][2];//����Ĵ�У׼ʱ  ���������
	uint8_t cnt = 0;//�ڼ��ε��
	tp_dev.state = 0;//�����豸�ĵ��״̬
	
	//��¼ʱ��
	uint16_t outtime = 0;
	//������ÿ������֮��ľ���
	uint32_t d1 = 0;
	uint32_t d2 = 0;
	//��������������γɵ�ֱ�ǵ������ߵľ��룬�������б������������ľ���
	//���d1��d2��
	uint32_t tem1 = 0;
	uint32_t tem2 = 0;
	
	//���
	float fac = 0;
	
	lcdDev.LCD_Clear();
	
	LCD_ShowStr(100,40,"TP Adjust",16,0);
	TP_Draw_Touch_Point(20,20,RED);//����1��У׼��
	
	while(1){
		TP_Scan(1);
		if((tp_dev.state&0xc0) == TP_CHAR_PRESS){
			outtime=0;
			tp_dev.state &= ~(1<<6);//������λ
			
			pos_tmp[cnt][0] = tp_dev.x[0];
			pos_tmp[cnt][1] = tp_dev.y[0];
			cnt++;
			
			switch(cnt){
				case 1:
					TP_Draw_Touch_Point(20,20,WHITE);//�����1��У׼��
					TP_Draw_Touch_Point(lcdDev.width-20,20,RED);//����2��У׼��
					break;
				case 2:
					TP_Draw_Touch_Point(lcdDev.width-20,20,WHITE);//�����2��У׼��
					TP_Draw_Touch_Point(20,lcdDev.height-20,RED);//����3��У׼��
					break;
				case 3:
					TP_Draw_Touch_Point(20,lcdDev.height-20,WHITE);//�����3��У׼��
					TP_Draw_Touch_Point(lcdDev.width-20,lcdDev.height-20,RED);//����4��У׼��
					break;
				case 4:{
					TP_Draw_Touch_Point(lcdDev.width-20,lcdDev.height-20,WHITE);//�����4��У׼��
					
					
					//�鿴���У׼���ĸ���  �����Ƿ����
					tem1=abs(pos_tmp[0][0]-pos_tmp[2][0]);//x1-x3
					tem2=abs(pos_tmp[0][1]-pos_tmp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,3�ľ���
					printf("1:  tem1=%d, tem2=%d\n",tem1,tem2);
					
					tem1=abs(pos_tmp[1][0]-pos_tmp[3][0]);//x2-x4
					tem2=abs(pos_tmp[1][1]-pos_tmp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,4�ľ���
					printf("2:  tem1=%d, tem2=%d\n",tem1,tem2);
					
					printf("��һ���㣺(%d,%d)���ڶ����㣺(%d,%d)���������㣺(%d,%d)�����ĸ��㣺(%d,%d)\n",pos_tmp[0][0],pos_tmp[0][1],pos_tmp[1][0],pos_tmp[1][1],pos_tmp[2][0],pos_tmp[2][1],pos_tmp[3][0],pos_tmp[3][1]);//��ʾ����   
					
					//����״̬�£�d1��d2�Ĵ�СӦ����ͬ�������d1/d2��Ϊһ��������
					fac=(float)d1/d2;	
					printf("d1=%d, d2=%d,  fac=%f\n",d1,d2,fac);
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						printf("1\n");
						cnt=0;
						lcdDev.LCD_Clear();
   	 				TP_Draw_Touch_Point(20,20,RED);								//�����1��У׼��
 						LCD_ShowStr(100,40,"failed to adjust!",16,1);
						continue;
					}
					
					//�鿴���У׼���ĸ���  ����Ƿ����
					tem1=abs(pos_tmp[0][0]-pos_tmp[1][0]);//x1-x3
					tem2=abs(pos_tmp[0][1]-pos_tmp[1][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,3�ľ���
					
					tem1=abs(pos_tmp[2][0]-pos_tmp[3][0]);//x2-x4
					tem2=abs(pos_tmp[2][1]-pos_tmp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						printf("2\n");
						cnt=0;
						lcdDev.LCD_Clear();
   	 				TP_Draw_Touch_Point(20,20,RED);								//����1
 						LCD_ShowStr(100,40,"failed to adjust!",16,1);
						//TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
						continue;
					}				

					//�鿴���У׼���ĸ���  �Խ����Ƿ����
					tem1=abs(pos_tmp[1][0]-pos_tmp[2][0]);//x1-x3
					tem2=abs(pos_tmp[1][1]-pos_tmp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,4�ľ���
	
					tem1=abs(pos_tmp[0][0]-pos_tmp[3][0]);//x2-x4
					tem2=abs(pos_tmp[0][1]-pos_tmp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,3�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						printf("3\n");
						cnt=0;
						lcdDev.LCD_Clear();
   	 				TP_Draw_Touch_Point(20,20,RED);								//����1
						LCD_ShowStr(100,40,"failed to adjust!",16,1);
 						//TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
						continue;
					}//��ȷ��					
					
					tp_dev.xfac=(float)(lcdDev.width-40)/(pos_tmp[1][0]-pos_tmp[0][0]);//�õ�xfac		 
					tp_dev.xoff=(lcdDev.width-tp_dev.xfac*(pos_tmp[1][0]+pos_tmp[0][0]))/2;//�õ�xoff
						  
					tp_dev.yfac=(float)(lcdDev.height-40)/(pos_tmp[2][1]-pos_tmp[0][1]);//�õ�yfac
					tp_dev.yoff=(lcdDev.height-tp_dev.yfac*(pos_tmp[2][1]+pos_tmp[0][1]))/2;//�õ�yoff  
					if(abs((int)tp_dev.xfac)>2||abs((int)tp_dev.yfac)>2){
						//�豸�ĺ��������÷���
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
		
		//ʮ��û�е�����Զ��Ƴ�
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
mode :0 ��ȡ��Ļ����    1 ��ȡ��������
*/
uint8_t TP_Scan(uint8_t mode){
	if(T_INT==0){
		tp_read_xy2(&tp_dev.x[0],&tp_dev.y[0]);//Ĭ�϶�ȡ�ľ�����������
		if(mode==0){
			tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//�����ת��Ϊ��Ļ����
			tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff;  
		}
		
		if((tp_dev.state&TP_PRESS_DOWN)==0){//֮ǰû�а�����Ļ
			
			tp_dev.state = TP_PRESS_DOWN | TP_CHAR_PRESS;
			tp_dev.x[4] = tp_dev.x[0];
			tp_dev.y[4] = tp_dev.y[0];
		}
	}else{
		if(tp_dev.state&TP_PRESS_DOWN){//֮ǰ�а�����Ļ
			tp_dev.state &= ~(1<<7);
		}else{//֮ǰû�а�����Ļ
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
