#include "AT24C16.h"

void AT24C16_Init(SoftI2C_Bus *bus)
{
    Software_I2C_Init(bus);
}

// 根据字节地址计算设备地址: 0xA0 | (bit10..8 << 1)
static uint8_t AT24C16_GetDevAddr(uint16_t addr)
{
    return AT24C16_BASE_ADDR | (((addr >> 8) & 0x07) << 1);
}

// 等待 EEPROM 写周期完成 (ACK polling)
static uint8_t AT24C16_WaitReady(SoftI2C_Bus *bus)
{
    uint32_t timeout = 0;
    while (timeout < 1000)
    {
        Software_I2C_Start(bus);
        Software_I2C_WriteByte(bus, AT24C16_BASE_ADDR); // 任意块地址均可
        if (Software_I2C_WaitACK(bus) == HAL_OK)
        {
            Software_I2C_Stop(bus);
            return 0; // OK
        }
        timeout++;
    }
    Software_I2C_Stop(bus);
    return 1; // 超时
}

uint8_t AT24C16_ReadByte(SoftI2C_Bus *bus, uint16_t addr, uint8_t *data)
{
    if (addr >= AT24C16_SIZE) return 1;

    uint8_t devAddr = AT24C16_GetDevAddr(addr);

    Software_I2C_Start(bus);
    Software_I2C_WriteByte(bus, devAddr);          // 设备地址 + 写
    if (Software_I2C_WaitACK(bus) != HAL_OK) goto err;
    Software_I2C_WriteByte(bus, (uint8_t)addr);    // 字地址 (低8位)
    if (Software_I2C_WaitACK(bus) != HAL_OK) goto err;

    Software_I2C_Start(bus);                       // 重复起始
    Software_I2C_WriteByte(bus, devAddr | 0x01);   // 设备地址 + 读
    if (Software_I2C_WaitACK(bus) != HAL_OK) goto err;
    *data = Software_I2C_ReadByte(bus);
    Software_I2C_SendNACK(bus);
    Software_I2C_Stop(bus);
    return 0;

err:
    Software_I2C_Stop(bus);
    return 1;
}

uint8_t AT24C16_WriteByte(SoftI2C_Bus *bus, uint16_t addr, uint8_t data)
{
    if (addr >= AT24C16_SIZE) return 1;

    uint8_t devAddr = AT24C16_GetDevAddr(addr);

    Software_I2C_Start(bus);
    Software_I2C_WriteByte(bus, devAddr);
    if (Software_I2C_WaitACK(bus) != HAL_OK) goto err;
    Software_I2C_WriteByte(bus, (uint8_t)addr);
    if (Software_I2C_WaitACK(bus) != HAL_OK) goto err;
    Software_I2C_WriteByte(bus, data);
    if (Software_I2C_WaitACK(bus) != HAL_OK) goto err;
    Software_I2C_Stop(bus);

    AT24C16_WaitReady(bus);
    return 0;

err:
    Software_I2C_Stop(bus);
    return 1;
}

uint8_t AT24C16_Read(SoftI2C_Bus *bus, uint16_t addr, uint8_t *buf, uint16_t len)
{
    if (addr + len > AT24C16_SIZE) return 1;

    uint8_t devAddr = AT24C16_GetDevAddr(addr);

    Software_I2C_Start(bus);
    Software_I2C_WriteByte(bus, devAddr);
    if (Software_I2C_WaitACK(bus) != HAL_OK) goto err;
    Software_I2C_WriteByte(bus, (uint8_t)addr);
    if (Software_I2C_WaitACK(bus) != HAL_OK) goto err;

    Software_I2C_Start(bus);
    Software_I2C_WriteByte(bus, devAddr | 0x01);
    if (Software_I2C_WaitACK(bus) != HAL_OK) goto err;

    for (uint16_t i = 0; i < len; i++)
    {
        buf[i] = Software_I2C_ReadByte(bus);
        if (i < len - 1)
            Software_I2C_SendACK(bus);
        else
            Software_I2C_SendNACK(bus);
    }
    Software_I2C_Stop(bus);
    return 0;

err:
    Software_I2C_Stop(bus);
    return 1;
}

// 页写入: addr 必须页对齐, len <= 剩余页空间
static uint8_t AT24C16_WritePage(SoftI2C_Bus *bus, uint16_t addr, const uint8_t *buf, uint16_t len)
{
    uint8_t devAddr = AT24C16_GetDevAddr(addr);

    Software_I2C_Start(bus);
    Software_I2C_WriteByte(bus, devAddr);
    if (Software_I2C_WaitACK(bus) != HAL_OK) goto err;
    Software_I2C_WriteByte(bus, (uint8_t)addr);
    if (Software_I2C_WaitACK(bus) != HAL_OK) goto err;

    for (uint16_t i = 0; i < len; i++)
    {
        Software_I2C_WriteByte(bus, buf[i]);
        if (Software_I2C_WaitACK(bus) != HAL_OK) goto err;
    }
    Software_I2C_Stop(bus);
    AT24C16_WaitReady(bus);
    return 0;

err:
    Software_I2C_Stop(bus);
    return 1;
}

uint8_t AT24C16_Write(SoftI2C_Bus *bus, uint16_t addr, const uint8_t *buf, uint16_t len)
{
    if (addr + len > AT24C16_SIZE) return 1;

    uint16_t written = 0;
    while (written < len)
    {
        // 当前地址在页内的剩余空间
        uint16_t pageRemain = AT24C16_PAGE_SIZE - ((addr + written) % AT24C16_PAGE_SIZE);
        uint16_t toWrite = len - written;
        if (toWrite > pageRemain) toWrite = pageRemain;

        if (AT24C16_WritePage(bus, addr + written, buf + written, toWrite) != 0)
            return 1;

        written += toWrite;
    }
    return 0;
}

// 自检: 写入测试值再读回比较
uint8_t AT24C16_Check(SoftI2C_Bus *bus)
{
    uint8_t testAddr = 0x00;
    uint8_t backup, readback;

    if (AT24C16_ReadByte(bus, testAddr, &backup) != 0) return 1;
    if (AT24C16_WriteByte(bus, testAddr, 0x55) != 0) return 1;
    if (AT24C16_ReadByte(bus, testAddr, &readback) != 0) return 1;
    if (readback != 0x55) return 1;
    AT24C16_WriteByte(bus, testAddr, backup); // 恢复原值
    return 0;
}
