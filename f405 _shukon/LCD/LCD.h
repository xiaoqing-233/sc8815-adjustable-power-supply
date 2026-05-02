#ifndef __LCD_DRIVER_H
#define __LCD_DRIVER_H

#include "main.h"       // 包含CubeMX生成的引脚定义
#include "spi.h"        // 包含SPI句柄定义

// ==========================================
// 1. 用户硬件配置 (根据你的CubeMX修改)
// ==========================================

// 使用的 SPI 句柄 (通常是 hspi1 或 hspi2)
extern SPI_HandleTypeDef hspi1;
#define LCD_SPI_HANDLE  &hspi1

// 引脚定义 (如果你在CubeMX里设置了User Label，main.h会自动生成这些宏)
//// 如果没有，请手动修改为 GPIOA, GPIO_PIN_x
//#define LCD_RES_GPIO_Port   LCD_RST_GPIO_Port
//#define LCD_RES_Pin         LCD_RST_Pin

//#define LCD_DC_GPIO_Port    LCD_DC_GPIO_Port
//#define LCD_DC_Pin          LCD_DC_Pin

//#define LCD_CS_GPIO_Port    LCD_CS_GPIO_Port
//#define LCD_CS_Pin          LCD_CS_Pin

// ==========================================
// 2. 引脚操作宏 (HAL库版)
// ==========================================
#define LCD_RES_Clr()  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET)
#define LCD_RES_Set()  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET)




#define LCD_DC_Clr()   HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET)
#define LCD_DC_Set()   HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET)

#define LCD_CS_Clr()   LCD_CS_GPIO_Port->BSRR = (uint32_t)LCD_CS_Pin << 16U
#define LCD_CS_Set()  LCD_CS_GPIO_Port->BSRR = LCD_CS_Pin

// ==========================================
// 3. 屏幕参数与颜色
// ==========================================
#define LCD_W 320
#define LCD_H 240

#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define BRED        0XF81F
#define GRED        0XFFE0
#define GBLUE       0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN       0XBC40 
#define BRRED       0XFC07 
#define GRAY        0X8430 

// ==========================================
// 4. 函数声明
// ==========================================
void LCD_Init(void);
void LCD_Write_Cmd(uint8_t cmd);

void LCD_Clear(uint16_t Color);
void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color);
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_ShowChinese16x16(uint16_t x, uint16_t y, uint16_t color, uint8_t index);
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
#endif

