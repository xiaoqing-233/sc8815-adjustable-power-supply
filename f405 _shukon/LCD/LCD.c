#include "LCD.h"
#include "lvgl.h"
#include "spi.h" // 确保包含 hspi1 定义

// ====================================================================
// 底层通信函数 (硬件NSS版)
// ====================================================================

/**
 * @brief  向SPI发送一个字节
 * @note   在硬件NSS模式下，HAL_SPI_Transmit 会自动拉低NSS，发送完拉高。
 */
static void LCD_Writ_Bus(uint8_t data)
{
    // Timeout 设为 100ms
    HAL_SPI_Transmit(&hspi1, &data, 1, 100);
}

/**
 * @brief  写8位命令
 */
void LCD_Write_Cmd(uint8_t cmd)
{
    // 移除软件 CS 操作
    // LCD_CS_Clr(); 
    
    LCD_DC_Clr();  // 命令模式
    LCD_Writ_Bus(cmd);
    
    // 移除软件 CS 操作
    // LCD_CS_Set(); 
}

/**
 * @brief  写8位数据
 */
void LCD_Write_Data(uint8_t data)
{
    // LCD_CS_Clr();
    
    LCD_DC_Set();  // 数据模式
    LCD_Writ_Bus(data);
    
    // LCD_CS_Set();
}

/**
 * @brief  写16位数据 (颜色值)
 * @note   硬件NSS模式下，必须一次性发送2个字节，保证中间CS不拉高。
 * 如果分两次发送，CS会在中间跳变，导致 ST7789 无法正确识别16位颜色。
 */
void LCD_Write_Data16(uint16_t data)
{
    uint8_t data_buff[2];
    data_buff[0] = data >> 8;
    data_buff[1] = data;

    // LCD_CS_Clr();
    LCD_DC_Set();
    
    // 关键：一次传输2字节，保证 NSS 在这两个字节期间保持低电平
    HAL_SPI_Transmit(&hspi1, data_buff, 2, 100);
    
    // LCD_CS_Set();
}

// ====================================================================
// 基础功能函数
// ====================================================================

/**
 * @brief  设置光标位置/显示区域
 */
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // 偏移量调整 (根据实际屏幕调整)
    x1 += 0; x2 += 0;
    y1 += 0; y2 += 0;

    // ST7789 允许在写参数过程中 CS 跳变，所以这里分开写也是可以的
    // 如果硬件 NSS 导致问题，可以将参数打包成数组一次发送

    LCD_Write_Cmd(0x2a); 
    LCD_Write_Data(x1 >> 8); LCD_Write_Data(x1);
    LCD_Write_Data(x2 >> 8); LCD_Write_Data(x2);

    LCD_Write_Cmd(0x2b); 
    LCD_Write_Data(y1 >> 8); LCD_Write_Data(y1);
    LCD_Write_Data(y2 >> 8); LCD_Write_Data(y2);

    LCD_Write_Cmd(0x2c); 
}

/**
 * @brief  屏幕初始化
 */
void LCD_Init(void)
{
    // 复位操作通常还是用 GPIO 控制 (RES 引脚不是 SPI 的一部分)
    LCD_RES_Clr();
    HAL_Delay(10);
    LCD_RES_Set();
    HAL_Delay(10);

    // ST7789 初始化序列
    LCD_Write_Cmd(0x3A); LCD_Write_Data(0x05);
    LCD_Write_Cmd(0xC5); LCD_Write_Data(0x1A);
    LCD_Write_Cmd(0x36); 
    LCD_Write_Data(0x60); // 显存访问控制

    // Porch Setting
    LCD_Write_Cmd(0xB2); 
    LCD_Write_Data(0x05); LCD_Write_Data(0x05); LCD_Write_Data(0x00);
    LCD_Write_Data(0x33); LCD_Write_Data(0x33);
    
    LCD_Write_Cmd(0xB7); LCD_Write_Data(0x05);
    LCD_Write_Cmd(0xBB); LCD_Write_Data(0x3F);
    LCD_Write_Cmd(0xC0); LCD_Write_Data(0x2C);
    LCD_Write_Cmd(0xC2); LCD_Write_Data(0x01);
    LCD_Write_Cmd(0xC3); LCD_Write_Data(0x0F);
    LCD_Write_Cmd(0xC4); LCD_Write_Data(0x20);
    LCD_Write_Cmd(0xC6); LCD_Write_Data(0x01);
    
    LCD_Write_Cmd(0xD0); LCD_Write_Data(0xA4); LCD_Write_Data(0xA1);
    LCD_Write_Cmd(0xE8); LCD_Write_Data(0x03);
    LCD_Write_Cmd(0xE9); 
    LCD_Write_Data(0x09); LCD_Write_Data(0x09); LCD_Write_Data(0x08);
    
    // Gamma Setting
    LCD_Write_Cmd(0xE0);
    LCD_Write_Data(0xD0); LCD_Write_Data(0x05); LCD_Write_Data(0x09); LCD_Write_Data(0x09);
    LCD_Write_Data(0x08); LCD_Write_Data(0x14); LCD_Write_Data(0x28); LCD_Write_Data(0x33);
    LCD_Write_Data(0x3F); LCD_Write_Data(0x07); LCD_Write_Data(0x13); LCD_Write_Data(0x14);
    LCD_Write_Data(0x28); LCD_Write_Data(0x30);
    
    LCD_Write_Cmd(0xE1);
    LCD_Write_Data(0xD0); LCD_Write_Data(0x05); LCD_Write_Data(0x09); LCD_Write_Data(0x09);
    LCD_Write_Data(0x08); LCD_Write_Data(0x03); LCD_Write_Data(0x24); LCD_Write_Data(0x32);
    LCD_Write_Data(0x32); LCD_Write_Data(0x3B); LCD_Write_Data(0x14); LCD_Write_Data(0x13);
    LCD_Write_Data(0x28); LCD_Write_Data(0x2F);

    LCD_Write_Cmd(0x11); // Sleep Out
    HAL_Delay(10);
    LCD_Write_Cmd(0x29); // Display On
}

/**
 * @brief  全屏填充/清屏
 */
void LCD_Clear(uint16_t Color)
{
    LCD_Fill(0, 0, LCD_W, LCD_H, Color);
}

/**
 * @brief  区域填充 (阻塞式，适配硬件NSS)
 * @note   在硬件NSS模式下，如果使用循环单点写入，CS会频繁跳变，速度极慢。
 * 这里必须使用 buffer 进行批量发送。
 */
void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{
    uint32_t size = (xend - xsta) * (yend - ysta);
    uint32_t i;
    
    // 1. 设置写窗口
    LCD_Address_Set(xsta, ysta, xend - 1, yend - 1);

    // 2. 准备小端转大端颜色
    uint8_t data[2];
    data[0] = color >> 8;
    data[1] = color;

    // LCD_CS_Clr(); // 硬件自动管理
    LCD_DC_Set();

    // 3. 这里的效率优化至关重要
    // 硬件 NSS 模式下，每调用一次 HAL_SPI_Transmit，CS 就会拉高拉低一次。
    // 如果像下面这样写：
    // for(i=0; i<size; i++) HAL_SPI_Transmit(..., 2, ...);
    // 屏幕刷新会非常慢，因为硬件有空闲等待时间。
    
    // 建议方案：使用栈上缓冲区或者复用你之前定义的 lcd_dma_buffer (即使不用DMA)
    // 如果内存允许，尽量一次多发点。
    
    // 简单阻塞方案 (由于没有开启DMA，这里只能循环发送，但为了速度，建议建立临时Buffer)
    #define FILL_CHUNK_SIZE 64 // 每次发64个像素
    uint8_t buffer[FILL_CHUNK_SIZE * 2]; 
    
    // 预先填充Buffer
    for(i = 0; i < FILL_CHUNK_SIZE; i++) {
        buffer[i*2] = data[0];
        buffer[i*2+1] = data[1];
    }

    uint32_t fill_chunks = size / FILL_CHUNK_SIZE;
    uint32_t remain = size % FILL_CHUNK_SIZE;

    // 批量发送
    for(i = 0; i < fill_chunks; i++) {
        HAL_SPI_Transmit(&hspi1, buffer, FILL_CHUNK_SIZE * 2, 100);
    }
    
    // 发送剩余部分
    if(remain > 0) {
        HAL_SPI_Transmit(&hspi1, buffer, remain * 2, 100);
    }

    // LCD_CS_Set(); // 硬件自动管理
}