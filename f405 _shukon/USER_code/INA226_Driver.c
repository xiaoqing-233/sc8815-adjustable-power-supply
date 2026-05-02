#include "INA226_Driver.h"
#include <math.h>
#include <stdio.h>
#include <string.h>


extern SoftI2C_Bus Bus_INA;
/* ============================================================
   底层读写函数适配 (调用 my_i2c.h 里的接口)
   ============================================================ */

// 写 16位寄存器 (MSB First)
static void INA226_WriteReg(uint8_t reg_addr, uint16_t data)
{
    Software_I2C_Start(&Bus_INA);           // 1. 起始
    Software_I2C_WriteByte(&Bus_INA,INA_ADDR); // 2. 发送设备地址 (写)
    Software_I2C_WaitACK(&Bus_INA);         // 3. 等待ACK
    
    Software_I2C_WriteByte(&Bus_INA,reg_addr); // 4. 发送寄存器地址
    Software_I2C_WaitACK(&Bus_INA);
    
    Software_I2C_WriteByte(&Bus_INA,data >> 8); // 5. 发送高8位
    Software_I2C_WaitACK(&Bus_INA);
    
    Software_I2C_WriteByte(&Bus_INA,data & 0xFF); // 6. 发送低8位
    Software_I2C_WaitACK(&Bus_INA);
    
    Software_I2C_Stop(&Bus_INA);            // 7. 停止
}

// 读 16位寄存器 (MSB First)
static uint16_t INA226_ReadReg(uint8_t reg_addr)
{
    uint8_t high_byte, low_byte;
    
    // --- 第一步：写入要读的寄存器地址 ---
    Software_I2C_Start(&Bus_INA);
    Software_I2C_WriteByte(&Bus_INA,INA_ADDR); // 写模式
    Software_I2C_WaitACK(&Bus_INA);
    
    Software_I2C_WriteByte(&Bus_INA,reg_addr); // 目标寄存器
    Software_I2C_WaitACK(&Bus_INA);
    
    // --- 第二步：重启总线读取数据 ---
    Software_I2C_Start(&Bus_INA);             // 重复起始 (Restart)
    Software_I2C_WriteByte(&Bus_INA,INA_ADDR | 0x01); // 读模式 (地址最后一位置1)
    Software_I2C_WaitACK(&Bus_INA);
    
    high_byte = Software_I2C_ReadByte(&Bus_INA); // 读高8位
    Software_I2C_SendACK(&Bus_INA);              // 主机给ACK，表示还想读
    
    low_byte = Software_I2C_ReadByte(&Bus_INA);  // 读低8位
    Software_I2C_SendNACK(&Bus_INA);             // 主机给NACK，表示不读了
    
    Software_I2C_Stop(&Bus_INA);                 // 停止
    
    return (uint16_t)((high_byte << 8) | low_byte);
}

/* ============================================================
   功能函数 (逻辑保持不变，只去掉 hi2c 句柄)
   ============================================================ */

static void INA226_CalculateCalibration(INA226_HandleTypeDef* dev, float max_current, float shunt_resistance) {
    dev->current_lsb = max_current / 32768.0f;
    float cal_value = 0.00512f / (dev->current_lsb * shunt_resistance);
    
    cal_value = (float)((int)cal_value);
    if (cal_value < 1.0f) cal_value = 1.0f;
    if (cal_value > 65535.0f) cal_value = 65535.0f;
    
    dev->calibration = (uint16_t)cal_value;
    dev->power_lsb = 25.0f * dev->current_lsb;
}

void INA226_Init(INA226_HandleTypeDef* dev, uint8_t address, float max_current, float shunt_resistance) {
    dev->address = address;
    INA226_CalculateCalibration(dev, max_current, shunt_resistance);
    
    // 写入配置
    INA226_WriteReg(INA226_CONFIG_REG, Conf_REG_Data);
    // 写入校准
    INA226_WriteReg(INA226_CALIB_REG, dev->calibration);
}

float INA226_ReadBusVoltage(INA226_HandleTypeDef* dev) {
    uint16_t raw = INA226_ReadReg(INA226_BUS_VOLTAGE_REG);
    return (float)raw * 0.00125f; 
}

float INA226_ReadShuntVoltage(INA226_HandleTypeDef* dev) {
    int16_t raw = (int16_t)INA226_ReadReg(INA226_SHUNT_VOLTAGE_REG);
    return (float)raw * 0.0000025f; 
}

float INA226_ReadCurrent(INA226_HandleTypeDef* dev) {
    int16_t raw = (int16_t)INA226_ReadReg(INA226_CURRENT_REG);
    return (float)raw * dev->current_lsb;
}

float INA226_ReadPower(INA226_HandleTypeDef* dev) {
    uint16_t raw = INA226_ReadReg(INA226_POWER_REG);
    return (float)raw * dev->power_lsb;
}

void PrintINA226Data(INA226_HandleTypeDef* dev, UART_HandleTypeDef* huart) {
    char buffer[256];

    float v_bus = INA226_ReadBusVoltage(dev);
    float current = INA226_ReadCurrent(dev);
    float power = INA226_ReadPower(dev);
    
    // 调试：读回配置和校准值
    uint16_t reg_conf = INA226_ReadReg(INA226_CONFIG_REG);
    uint16_t reg_cal  = INA226_ReadReg(INA226_CALIB_REG);

    sprintf(buffer, 
        "INA226 [0x%02X]:\r\n"
        " Vbus: %.3f V\r\n"
        " Curr: %.3f A\r\n"
        " Powr: %.3f W\r\n"
        " (Debug: Conf=0x%04X, Cal=0x%04X)\r\n", 
        dev->address, v_bus, current, power, reg_conf, reg_cal);

    HAL_UART_Transmit(huart, (uint8_t*)buffer, strlen(buffer), 100);
}