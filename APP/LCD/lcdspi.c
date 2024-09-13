#include "lcdspi.h"
#include "stdio.h"

SPI_HandleTypeDef lcd_spi_handler;

void lcd_spi_init(void){
	
	GPIO_InitTypeDef gpio_init_struct;
	
	
	LCD_SPI_SCK_GPIO_CLK_ENABLE();
	LCD_SPI_MISO_GPIO_CLK_ENABLE();
	LCD_SPI_MOSI_GPIO_CLK_ENABLE();
	LCD_SPI_CS_GPIO_CLK_ENABLE();
	LCD_SPI_INT_GPIO_CLK_ENABLE();
	
	gpio_init_struct.Pin = LCD_SPI_INT_GPIO_PIN;
	gpio_init_struct.Pull = GPIO_PULLUP;
	gpio_init_struct.Mode = GPIO_MODE_INPUT;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LCD_SPI_INT_GPIO_PORT,&gpio_init_struct);

	gpio_init_struct.Pin = LCD_SPI_MISO_GPIO_PIN;
	HAL_GPIO_Init(LCD_SPI_MISO_GPIO_PORT,&gpio_init_struct);
	
	
	gpio_init_struct.Pin = LCD_SPI_MOSI_GPIO_PIN;
	gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(LCD_SPI_MOSI_GPIO_PORT,&gpio_init_struct);
	
	gpio_init_struct.Pin = LCD_SPI_CS_GPIO_PIN;
	HAL_GPIO_Init(LCD_SPI_CS_GPIO_PORT,&gpio_init_struct);
	
	gpio_init_struct.Pin = LCD_SPI_SCK_GPIO_PIN;
	HAL_GPIO_Init(LCD_SPI_SCK_GPIO_PORT,&gpio_init_struct);
	
}

void lcd_spi_write_byte(uint8_t data){
	for(uint8_t count=0; count<8; count++){
		if(data & 0x80){
			T_MOSI(1);
		}else{
			T_MOSI(0);
		}
		data <<= 1;
		T_CLK(1);
		delay_us(1);
		T_CLK(0);
	}
}

uint16_t lcd_spi_read_byte(uint8_t cmd){
    uint8_t count = 0;
    uint16_t num = 0;
    T_CLK(0);           /* ������ʱ�� */
    T_MOSI(0);          /* ���������� */
    T_CS(0);            /* ѡ�д�����IC */
    lcd_spi_write_byte(cmd); /* ���������� */
    delay_us(6);        /* ADS7846��ת��ʱ���Ϊ6us */
    T_CLK(0);
    delay_us(1);
    T_CLK(1);           /* ��1��ʱ�ӣ����BUSY */
    delay_us(1);
    T_CLK(0);
	
    for (count = 0; count < 16; count++)    /* ����16λ����,ֻ�и�12λ��Ч */
    {
        num <<= 1;
        T_CLK(0);       /* �½�����Ч */
        delay_us(1);
        T_CLK(1);

        if (T_MISO)num++;
    }

    num >>= 4;          /* ֻ�и�12λ��Ч. */
    T_CS(1);            /* �ͷ�Ƭѡ */
		//printf("num=%d\n", num);
    return num;	
}












