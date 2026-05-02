/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LCD.h"
#include "GUI.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "ui.h"
#include "lv_demo_widgets.h"
#include "INA226_Driver.h"
#include "ntc_temp.h"
#include "soft_i2c.h"

#include "PID.h"
#include "SC8815.h"
#include "string.h"
#include "stdio.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern uint8_t tim_num;
extern uint8_t PID_tim;
extern uint16_t Update_lcd_tim;

char msg[25];  // uart信息
extern uint8_t rx_one;
extern uint8_t Rx_state ;
extern uint8_t RxIndex ;
extern uint8_t RxBuffer[14];



float InputVoltage = 0; 
float OutputPower   = 0;
float tempture = 0.0f; // 温度变量
float vin_volts; //输入电压


INA226_HandleTypeDef INA_outData;	
PID_Type Voltage_PID;
PID_Type Current_PID;
PSU_Status_Type PSU;
uint8_t check_res = 0;

uint32_t lcd_refresh_time = 0;
uint32_t adc_get_time = 0;
uint32_t print_time = 0;

// 1. 定义 INA226 总线 (PB6, PB7)
SoftI2C_Bus Bus_INA = {GPIOB, GPIO_PIN_10, GPIOB, GPIO_PIN_11};

// 2. 定义 SC8815 总线 (PA8-SCL, PC9-SDA)
SoftI2C_Bus Bus_SC  = {GPIOA, GPIO_PIN_8, GPIOC, GPIO_PIN_9};
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
void On_Screen_Loaded(lv_event_t * e) {
    Update_All_Power_UI(); 
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile uint32_t debug_counter = 0;
volatile uint8_t debug_btn_state = 1;


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
  MX_DMA_Init();
  MX_TIM12_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_UART4_Init();
  MX_TIM11_Init();
  /* USER CODE BEGIN 2 */
//	__HAL_FLASH_INSTRUCTION_CACHE_ENABLE();

//// 2. �������ݻ��� (D-Cache, ��� Flash)
//	__HAL_FLASH_DATA_CACHE_ENABLE();

//// 3. ����Ԥȡָ�� (Prefetch)
//	__HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  HAL_TIM_Base_Start_IT(&htim12);
	lv_init();
//	LCD_Init();
	// 初始化lvgl显示设备
	lv_port_disp_init();


	lv_port_indev_init();

	ui_init();





	My_Waveform_Init();
	lv_obj_set_style_size(ui_Chart1, 0, LV_PART_INDICATOR);

	float max_current_OUT = 12.0f;
	

  lv_obj_add_event_cb(ui_PowerOpen, Switch_State_Changed, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(ui_Screen1, On_Screen_Loaded, LV_EVENT_SCREEN_LOADED, NULL);
  
	// 注册电压变化事件
	lv_obj_add_event_cb(ui_SpinboxVolt, On_SpinboxVolt_Changed, LV_EVENT_VALUE_CHANGED, NULL);

	// 注册电流变化事件
	lv_obj_add_event_cb(ui_SpinboxCurr, On_SpinboxCurr_Changed, LV_EVENT_VALUE_CHANGED, NULL);
	
	
	
  lv_obj_add_event_cb(ui_PowerOpen2, Switch2_State_Changed, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(ui_Screen2, On_Screen_Loaded, LV_EVENT_SCREEN_LOADED, NULL);
 
 // 给 Screen1 的控件注册回调
  lv_obj_add_event_cb(ui_SpinboxVolt, On_Spinbox_Changed, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(ui_SpinboxCurr, On_Spinbox_Changed, LV_EVENT_VALUE_CHANGED, NULL);

    // 给 Screen2 的控件注册回调
  lv_obj_add_event_cb(ui_SpinboxVolt2, On_Spinbox_Changed, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(ui_SpinboxCurr2, On_Spinbox_Changed, LV_EVENT_VALUE_CHANGED, NULL);

  Update_All_Power_UI();
	
	
	PID_Init();
	
  Software_I2C_Init(&Bus_INA);
  Software_I2C_Init(&Bus_SC);
	
	SC8815_Init_Check();
	SC8815_Start_OTG_Mode();
	
	INA226_Init(&INA_outData, INA_ADDR, max_current_OUT, shunt_res_initial);
	
	PSU.Set_Volts = 0.5f;  // 默认设定 0.5v
  PSU.Set_Amps  = 1.0f;  // 默认限流 0.3A
  PSU_Set_Output_State(0);
	
	lv_spinbox_set_value(ui_SpinboxVolt, (int32_t)(PSU.Set_Volts * 100));
  lv_spinbox_set_value(ui_SpinboxCurr, (int32_t)(PSU.Set_Amps * 100));
	Sync_Struct_To_UI();
	
	HAL_TIM_Base_Start_IT(&htim11);
	HAL_UART_Receive_IT(&huart4,&rx_one,1);
	BUZZER_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		

		if(tim_num >= 5)
		{
			lv_timer_handler();
			tim_num = 0;
			
		}
		if(PID_tim >= 5)
		{
			PSU.Real_Volts = INA226_ReadBusVoltage(&INA_outData);                  // 获取当前输出电压
      PSU.Real_Curr = fabs(INA226_ReadCurrent(&INA_outData));              // 获取当前输出电流
      // current_OutputPower = INA226_ReadPower(&OutputData);                      
      OutputPower = PSU.Real_Volts * PSU.Real_Curr;                          // 获取当前输出功率
			
			uint16_t stepVoltage = SC8815_GetOutputVoltageSetp();
			uint16_t outVoltage = SC8815_GetOutputVoltage();
			
			if(PSU.Set_Volts < 2.8)
			{
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET);
			}
			else
			{
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET);
			}
			
			
			
			
//			SC8815_GetOutputVoltageSetp();
			PSU_Control_Loop();
//		  char buffer[128];
//          sprintf(buffer, 
//			  " stepVoltage: %d mV\r\n"
//        " OUTbus: %.3f V\r\n"
//        " outVoltage: %d mV\r\n"
//        " Powr: %.3f W\r\n"
//         ,stepVoltage,OutputVoltage, outVoltage, OutputPower);

//			HAL_UART_Transmit(&huart4, (uint8_t*)buffer, strlen(buffer), 100);
			PID_tim = 0;
			
		}
		
		
		if(HAL_GetTick() - lcd_refresh_time > 200)
		{
			lcd_refresh_time = HAL_GetTick();
			uint16_t vin_mv = SC8815_Read_BATT_Voltage();
			
			vin_volts = (float)vin_mv / 1000.0f;
			Update_Main_Screen_Sensors(PSU.Real_Volts,PSU.Real_Curr,OutputPower,vin_volts);
			
			
			if(lv_scr_act() == ui_Screen1) 
			{
        lv_label_set_text_fmt(ui_VoltVal, "%.2fV", PSU.Real_Volts);
        lv_label_set_text_fmt(ui_CurrVal, "%.2fA", PSU.Real_Curr);
      
			}
    
    // 如果你在 Screen2 (曲线图界面)
			else if(lv_scr_act() == ui_Screen2) 
			{
				lv_label_set_text_fmt(ui_VoltVal, "%.2fV", PSU.Real_Volts);
        lv_label_set_text_fmt(ui_CurrVal, "%.2fA", PSU.Real_Curr);
        
			}
			
		}
		// 在 SC8815_SetOutputVoltage 之后加入打印
		
		if(HAL_GetTick() - print_time > 500) // 每 0.5秒打印一次
		{
				print_time = HAL_GetTick();
				char buffer[128];
				sprintf(buffer,"Set:%.2f Real:%.2f | C_PID_Out:%.2f \r\n", 
           PSU.Set_Amps, 
           PSU.Real_Curr, 
           Current_PID.Output);
			HAL_UART_Transmit(&huart4, (uint8_t*)buffer, strlen(buffer), 100);
		}
		if(HAL_GetTick() - adc_get_time > 500)
		{
			adc_get_time = HAL_GetTick();
			tempture = NTC_Read_Temperature(&hadc1);
			
			Update_Temp_UI(tempture);
			
		}
		
		//蜂鸣器任务处理函数
		BUZZER_Task_Handler();
		

// 
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
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
#ifdef USE_FULL_ASSERT
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
