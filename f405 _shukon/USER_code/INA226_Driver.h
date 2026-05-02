#ifndef INA226_DRIVER_H
#define INA226_DRIVER_H

#include <stdint.h>
#include "usart.h" 
// #include "i2c.h" // 不需要硬件I2C了
#include "soft_i2c.h" // 【新增】引入你的软件I2C头文件 (如果是 my_i2c.h 就改这里)

// 【关键修改】地址改为扫描到的 0x80 (即 0x40 << 1)
#define INA_ADDR              0x80 

// 寄存器地址保持不变
#define INA226_CONFIG_REG     0x00 
#define INA226_SHUNT_VOLTAGE_REG  0x01 
#define INA226_BUS_VOLTAGE_REG    0x02 
#define INA226_POWER_REG      0x03 
#define INA226_CURRENT_REG    0x04 
#define INA226_CALIB_REG      0x05 
#define INA226_MASK_ENABLE_REG    0x06 
#define INA226_ALERT_REG      0x07 
#define INA226_MANUF_ID_REG   0xFE 
#define INA226_DIE_ID_REG     0xFF 

// 配置值
#define Conf_REG_Data         0x4427 
#define shunt_res_initial     0.005 

// 【修改结构体】删掉 hi2c，只保留计算用的参数
typedef struct {
    // I2C_HandleTypeDef* hi2c; // 【删除】不再需要
    uint8_t address;            // 设备地址
    float current_lsb;          // 电流LSB
    float power_lsb;            // 功率LSB
    uint16_t calibration;       // 校准寄存器值
} INA226_HandleTypeDef;

// 函数声明 (去掉了 hi2c 参数)
void INA226_Init(INA226_HandleTypeDef* dev, uint8_t address, float max_current, float shunt_resistance);
float INA226_ReadBusVoltage(INA226_HandleTypeDef* dev);
float INA226_ReadShuntVoltage(INA226_HandleTypeDef* dev);
float INA226_ReadCurrent(INA226_HandleTypeDef* dev);
float INA226_ReadPower(INA226_HandleTypeDef* dev);
void PrintINA226Data(INA226_HandleTypeDef* dev, UART_HandleTypeDef* huart);

#endif