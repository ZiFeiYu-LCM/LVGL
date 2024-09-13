#ifndef SPI_H
#define SPI_H
#include "main.h"
#include "delay.h"


#define LCD_SPI_SCK_GPIO_PORT GPIOB
#define LCD_SPI_SCK_GPIO_PIN  GPIO_PIN_1
#define LCD_SPI_SCK_GPIO_CLK_ENABLE() do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0)


#define LCD_SPI_MISO_GPIO_PORT GPIOB
#define LCD_SPI_MISO_GPIO_PIN  GPIO_PIN_2
#define LCD_SPI_MISO_GPIO_CLK_ENABLE() do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0)


#define LCD_SPI_MOSI_GPIO_PORT GPIOF
#define LCD_SPI_MOSI_GPIO_PIN  GPIO_PIN_9
#define LCD_SPI_MOSI_GPIO_CLK_ENABLE() do{__HAL_RCC_GPIOF_CLK_ENABLE();}while(0)


#define LCD_SPI_CS_GPIO_PORT GPIOF
#define LCD_SPI_CS_GPIO_PIN  GPIO_PIN_11
#define LCD_SPI_CS_GPIO_CLK_ENABLE()   do{__HAL_RCC_GPIOF_CLK_ENABLE();}while(0)

#define LCD_SPI_INT_GPIO_PORT GPIOF
#define LCD_SPI_INT_GPIO_PIN  GPIO_PIN_10
#define LCD_SPI_INT_GPIO_CLK_ENABLE()  do{__HAL_RCC_GPIOF_CLK_ENABLE();}while(0)


#define T_INT           HAL_GPIO_ReadPin(LCD_SPI_INT_GPIO_PORT, LCD_SPI_INT_GPIO_PIN)           /* T_INT */
#define T_MISO          HAL_GPIO_ReadPin(LCD_SPI_MISO_GPIO_PORT, LCD_SPI_MISO_GPIO_PIN)         /* T_MISO */

#define T_MOSI(x)     do{ x ? \
                          HAL_GPIO_WritePin(LCD_SPI_MOSI_GPIO_PORT, LCD_SPI_MOSI_GPIO_PIN, GPIO_PIN_SET) : \
                          HAL_GPIO_WritePin(LCD_SPI_MOSI_GPIO_PORT, LCD_SPI_MOSI_GPIO_PIN, GPIO_PIN_RESET); \
                      }while(0)     /* T_MOSI */

#define T_CLK(x)      do{ x ? \
                          HAL_GPIO_WritePin(LCD_SPI_SCK_GPIO_PORT, LCD_SPI_SCK_GPIO_PIN, GPIO_PIN_SET) : \
                          HAL_GPIO_WritePin(LCD_SPI_SCK_GPIO_PORT, LCD_SPI_SCK_GPIO_PIN, GPIO_PIN_RESET); \
                      }while(0)     /* T_CLK */

#define T_CS(x)       do{ x ? \
                          HAL_GPIO_WritePin(LCD_SPI_CS_GPIO_PORT, LCD_SPI_CS_GPIO_PIN, GPIO_PIN_SET) : \
                          HAL_GPIO_WritePin(LCD_SPI_CS_GPIO_PORT, LCD_SPI_CS_GPIO_PIN, GPIO_PIN_RESET); \
                      }while(0)     /* T_CS */



/* SPI总线速度设置 */
#define SPI_SPEED_2         0
#define SPI_SPEED_4         1
#define SPI_SPEED_8         2
#define SPI_SPEED_16        3
#define SPI_SPEED_32        4
#define SPI_SPEED_64        5
#define SPI_SPEED_128       6
#define SPI_SPEED_256       7


void lcd_spi_init(void);
uint16_t lcd_spi_read_byte(uint8_t cmd);
void lcd_spi_write_byte(uint8_t data);

											

											
											
#endif





