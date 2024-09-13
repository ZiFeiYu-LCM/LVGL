/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mylcd.h"
#include <stdio.h>
#include "delay.h"
#include "lcdspi.h"
#include "touch.h"
#include "eeprom.h"
#include "i2c.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lv_mainstart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* TIM1 定时器回调函数*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
        // TIM1 更新中断处理代码
        // 例如：处理定时器溢出事件
			printf("123\n");
			//lv_task_handler();
			//lv_timer_handler();
			lv_tick_inc(1);
    }
}




/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FSMC_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  MX_SPI3_Init();
  MX_TIM1_Init();
	
  /* USER CODE BEGIN 2 */
	
	 // 获取系统时钟频率
	//void delay_init();
	//printf("System Clock Frequency: %lf Hz\r\n", cyclesPer);
	
	HAL_Delay(10);
	delay_init(72);
	
	//EEPROM_Init();
	lcd_spi_init();
	LCD_init();
	delay_ms(10);	

	

	HAL_TIM_Base_Start_IT(&htim1);
	lv_init();
	lv_port_disp_init();
	lv_port_indev_init();
	

	
	
//	lv_obj_t *label = lv_label_create(lv_scr_act());
//	lv_label_set_text(label,"Hellow world!");
//	lv_obj_center(label);
	
	/*
	printf("0x%04X\n",lcdDev.id);

	lcdDev.LCD_Clear();
	LCD_FillRect(0,0,200,450,GREEN);
	lcdDev.LCD_ShowStr(70,70,"Hello World! my name is Liu Chaoming",24,1);
	
	LCD_DrawLine(50,50,100,100, BLACK);
	LCD_DrawCircle(200,200, 100, BLUE,0);
	*/

	uint8_t flag = 0x21;
	if(EEPROM_Read_Byte(0x00FF) != flag){
		TP_Adjust();
		
		uint32_t xfac = FloatToUint32(tp_dev.xfac);
		uint32_t yfac = FloatToUint32(tp_dev.yfac);
		uint16_t xoff = ShortToUint16(tp_dev.xoff);
		uint16_t yoff = ShortToUint16(tp_dev.yoff);
		
		
		EEPROM_Write_Byte(0x0001,(uint8_t)((xfac&0xFF000000)>>24));
		EEPROM_Write_Byte(0x0002,(uint8_t)((xfac&0x00FF0000)>>16));
		EEPROM_Write_Byte(0x0003,(uint8_t)((xfac&0x0000FF00)>>8));
		EEPROM_Write_Byte(0x0004,(uint8_t)(xfac&0x000000FF));
		
		EEPROM_Write_Byte(0x0005,(uint8_t)((yfac&0xFF000000)>>24));
		EEPROM_Write_Byte(0x0006,(uint8_t)((yfac&0x00FF0000)>>16));
		EEPROM_Write_Byte(0x0007,(uint8_t)((yfac&0x0000FF00)>>8));
		EEPROM_Write_Byte(0x0008,(uint8_t)(yfac&0x000000FF));
		
		EEPROM_Write_Byte(0x0009,(uint8_t)((xoff&0xFF00)>>8));
		EEPROM_Write_Byte(0x000A,(uint8_t)(xoff&0x00FF));
		
		EEPROM_Write_Byte(0x000B,(uint8_t)((yoff&0xFF00)>>8));
		EEPROM_Write_Byte(0x000C,(uint8_t)(yoff&0x00FF));
		
		
		EEPROM_Write_Byte(0x00FF,flag);
		
		printf("tp_dev.xfac = %lf\n",tp_dev.xfac);
		printf("tp_dev.yfac = %lf\n",tp_dev.yfac);
		printf("tp_dev.xoff = %d\n",tp_dev.xoff);
		printf("tp_dev.yoff = %d\n",tp_dev.yoff);
		
	}else{
		uint32_t xfac;
		uint32_t yfac;
		uint16_t xoff;
		uint16_t yoff;
		
		
		xfac = (EEPROM_Read_Byte(0x0001)<<24)|(EEPROM_Read_Byte(0x0002)<<16)|(EEPROM_Read_Byte(0x0003)<<8)|EEPROM_Read_Byte(0x0004);		
		yfac = (EEPROM_Read_Byte(0x0005)<<24)|(EEPROM_Read_Byte(0x0006)<<16)|(EEPROM_Read_Byte(0x0007)<<8)|EEPROM_Read_Byte(0x0008);

		xoff = (EEPROM_Read_Byte(0x0009)<<8)|EEPROM_Read_Byte(0x000A);
		yoff = (EEPROM_Read_Byte(0x000B)<<8)|EEPROM_Read_Byte(0x000C);	
		
		tp_dev.xfac = Uint32ToFloat(xfac);
		tp_dev.yfac = Uint32ToFloat(yfac);
		tp_dev.xoff = Uint16ToShort(xoff);
		tp_dev.yoff = Uint16ToShort(yoff);
	
	}
	
	lcdDev.LCD_Clear();
	lv_mainstart();

	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		lv_task_handler();
		
		//lv_timer_handler();
		//lv_tick_inc(5);//这个必须调用，要么不会处理触摸屏事件
		
		/*
		TP_Scan(0);
		if((tp_dev.state&0xc0) == TP_CHAR_PRESS){
			tp_dev.state &= ~(1<<6);
			printf("x=%d  y=%d\n",tp_dev.x[4],tp_dev.y[4]);
			LCD_DrawCircle(tp_dev.x[4],tp_dev.y[4], 10, BLUE,1);
		}
		*/
		
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
