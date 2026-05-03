#ifndef __AT24C16_H
#define __AT24C16_H

#include "soft_i2c.h"

#define AT24C16_SIZE        2048    // 总容量 2KB
#define AT24C16_PAGE_SIZE   16      // 页大小 16字节
#define AT24C16_BLOCK_SIZE  256     // 每个块 256字节
#define AT24C16_BLOCK_NUM   8       // 共8个块
#define AT24C16_BASE_ADDR   0xA0    // I2C 基地址 (1010 0000)
#define AT24C16_WRITE_DELAY 5       // 写周期最大等待时间 ms

// EEPROM 存储地址布局
#define EE_ADDR_MAGIC       0x00    // 校验字节 (1B)
#define EE_ADDR_SET_VOLTS   0x01    // 设定电压 float (4B)
#define EE_ADDR_SET_AMPS    0x05    // 设定电流 float (4B)
#define EE_MAGIC_VALUE      0xA5    // 有效标志

void     AT24C16_Init(SoftI2C_Bus *bus);
uint8_t  AT24C16_ReadByte(SoftI2C_Bus *bus, uint16_t addr, uint8_t *data);
uint8_t  AT24C16_WriteByte(SoftI2C_Bus *bus, uint16_t addr, uint8_t data);
uint8_t  AT24C16_Read(SoftI2C_Bus *bus, uint16_t addr, uint8_t *buf, uint16_t len);
uint8_t  AT24C16_Write(SoftI2C_Bus *bus, uint16_t addr, const uint8_t *buf, uint16_t len);
uint8_t  AT24C16_Check(SoftI2C_Bus *bus);

#endif
