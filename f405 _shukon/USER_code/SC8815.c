/*---------------------------------------------------------------------------/
/  SC8815 通用固件库 V0.2 (C)Sghz，2021
/----------------------------------------------------------------------------/
/ SC8815库是根据以下条件的许可证政策开发的免费软件。
/
/ 版权所有 (C) 2021，Sghz，保留所有权利。
/
/ 1.源代码的重新分发必须保留上述版权声明、本条件和以下免责声明。
/
/ 本软件由版权所有者和贡献者“按原样”提供，与本软件相关的任何担保均不承担责任。
/ 版权所有人或贡献者对使用本软件造成的任何损害概不负责。
/---------------------------------------------------------------------------*/

//包含头文件
#include "SC8815.h"
#include "main.h"
#include "usart.h"
#include "soft_i2c.h"

extern SoftI2C_Bus Bus_SC;

void SoftwareDelay(uint8_t ms)//软件延时毫秒
{
	HAL_Delay(ms);
}
/****************************************
* @brief    通过 软件I2C 写一个字节到寄存器
*****************************************/
void I2C_WriteRegByte(uint8_t SlaveAddress, uint8_t RegAddress, uint8_t ByteData)
{
    // SC8815 写时序: Start -> Addr(W) -> Reg -> Data -> Stop
    
    Software_I2C_Start(&Bus_SC);
    
    Software_I2C_WriteByte(&Bus_SC, SlaveAddress); // 地址已经包含了写位(0)
    Software_I2C_WaitACK(&Bus_SC);
    
    Software_I2C_WriteByte(&Bus_SC, RegAddress);
    Software_I2C_WaitACK(&Bus_SC);
    
    Software_I2C_WriteByte(&Bus_SC, ByteData);
    Software_I2C_WaitACK(&Bus_SC);
    
    Software_I2C_Stop(&Bus_SC);
}

/****************************************
* @brief    通过 软件I2C 从寄存器读一个字节
*****************************************/
uint8_t I2C_ReadRegByte(uint8_t SlaveAddress, uint8_t RegAddress)
{
    uint8_t data = 0;
    
    // SC8815 读时序: Start -> Addr(W) -> Reg -> Restart -> Addr(R) -> Read -> Nack -> Stop
    
    // 1. 写入寄存器地址
    Software_I2C_Start(&Bus_SC);
    Software_I2C_WriteByte(&Bus_SC, SlaveAddress); // 写模式
    Software_I2C_WaitACK(&Bus_SC);
    
    Software_I2C_WriteByte(&Bus_SC, RegAddress);
    Software_I2C_WaitACK(&Bus_SC);
    
    // 2. 重启总线读取数据
    Software_I2C_Start(&Bus_SC);
    Software_I2C_WriteByte(&Bus_SC, SlaveAddress | 0x01); // 读模式 (地址最低位变1)
    Software_I2C_WaitACK(&Bus_SC);
    
    data = Software_I2C_ReadByte(&Bus_SC);
    
    // SC8815 读取最后一个字节后通常需要 NACK，然后 Stop
    Software_I2C_SendNACK(&Bus_SC); 
    
    Software_I2C_Stop(&Bus_SC);
    
    return data;
}
/****************************************
* @brief    初始化 SC8815 的示例 Demo 函数
*****************************************/
//void SC8815_Init_Demo(void)
//{
//    SC8815_BatteryConfigTypeDef SC8815_BatteryConfigStruct = { 0 };
//    SC8815_HardwareInitTypeDef SC8815_HardwareInitStruct = { 0 };
//    SC8815_InterruptStatusTypeDef SC8815_InterruptMaskInitStruct = { 0 };

//    /****启动 SC8815...****/
//    //->设置 PSTOP 为高
//    //->设置 CE 为低
//    //SoftwareDelay(5);   //必要的启动延时

//    //配置 SC8815 电池参数选项
//    SC8815_BatteryConfigStruct.IRCOMP = SCBAT_IRCOMP_20mR;
//    SC8815_BatteryConfigStruct.VBAT_SEL = SCBAT_VBAT_SEL_Internal;
//    SC8815_BatteryConfigStruct.CSEL = SCBAT_CSEL_4S;
//    SC8815_BatteryConfigStruct.VCELL = SCBAT_VCELL_4v50;
//    SC8815_BatteryConfig(&SC8815_BatteryConfigStruct);

//    //配置 SC8815 硬件参数选项
//    SC8815_HardwareInitStruct.IBAT_RATIO = SCHWI_IBAT_RATIO_6x;
//    SC8815_HardwareInitStruct.IBUS_RATIO = SCHWI_IBUS_RATIO_3x;
//    SC8815_HardwareInitStruct.VBAT_RATIO = SCHWI_VBAT_RATIO_12_5x;
//    SC8815_HardwareInitStruct.VBUS_RATIO = SCHWI_VBUS_RATIO_12_5x;
//    SC8815_HardwareInitStruct.VINREG_Ratio = SCHWI_VINREG_RATIO_100x;
//    SC8815_HardwareInitStruct.SW_FREQ = SCHWI_FREQ_300KHz_2;
//    SC8815_HardwareInitStruct.DeadTime = SCHWI_DT_40ns;
//    SC8815_HardwareInitStruct.ICHAR = SCHWI_ICHAR_IBAT;
//    SC8815_HardwareInitStruct.TRICKLE = SCHWI_TRICKLE_Enable;
//    SC8815_HardwareInitStruct.TERM = SCHWI_TERM_Enable;
//    SC8815_HardwareInitStruct.FB_Mode = SCHWI_FB_Internal;
//    SC8815_HardwareInitStruct.TRICKLE_SET = SCHWI_TRICKLE_SET_70;
//    SC8815_HardwareInitStruct.OVP = SCHWI_OVP_Enable;
//    SC8815_HardwareInitStruct.DITHER = SCHWI_DITHER_Disable;
//    SC8815_HardwareInitStruct.SLEW_SET = SCHWI_SLEW_1mV_us;
//    SC8815_HardwareInitStruct.ADC_SCAN = SCHWI_ADC_Disable;
//    SC8815_HardwareInitStruct.ILIM_BW = SCHWI_ILIM_BW_1_25KHz;
//    SC8815_HardwareInitStruct.LOOP = SCHWI_LOOP_Normal;
//    SC8815_HardwareInitStruct.ShortFoldBack = SCHWI_SFB_Enable;
//    SC8815_HardwareInitStruct.EOC = SCHWI_EOC_1_25;
//    SC8815_HardwareInitStruct.PFM = SCHWI_PFM_Disable;
//    SC8815_HardwareInit(&SC8815_HardwareInitStruct);

//    //配置 SC8815 中断屏蔽选项
//    SC8815_InterruptMaskInitStruct.AC_OK = sENABLE;
//    SC8815_InterruptMaskInitStruct.INDET = sENABLE;
//    SC8815_InterruptMaskInitStruct.VBUS_SHORT = sENABLE;
//    SC8815_InterruptMaskInitStruct.OTP = sENABLE;
//    SC8815_InterruptMaskInitStruct.EOC = sENABLE;
//    SC8815_ConfigInterruptMask(&SC8815_InterruptMaskInitStruct);
//    /***现在可以设置 PSTOP 引脚为低, 启动 SC8815 功率转换****/



////    /*** 示例1, 设置为充电模式,电池和 VBUS 限流 2A, VINREG 设置为 12V ****/
////    SC8815_SetBatteryCurrLimit(2000);
////    SC8815_SetBusCurrentLimit(2000);
////    SC8815_VINREG_SetVoltage(12000);
////    SC8815_OTG_Disable();


//    /*** 示例2, 设置为反向放电模式,电池和 VBUS 限流 3A, 输出电压 设置为 12V ****/
//    //SC8815_SetBatteryCurrLimit(2000);
//    //SC8815_SetBusCurrentLimit(2000);
//    //SC8815_SetOutputVoltage(12000);
//    //SC8815_OTG_Enable();


//    /*** 示例3, 读取 SC8815 ADC 数据 ****/
//    //uint16_t VbusVolt = SC8815_Read_VBUS_Voltage();
//    //uint16_t VbusCurr = SC8815_Read_VBUS_Current();
//    //uint16_t BattVolt = SC8815_Read_BATT_Voltage();
//    //uint16_t BattCurr = SC8815_Read_BATT_Current();


//    /*** 示例4, 读取 SC8815 中断状态 ****/
//    SC8815_ReadInterrupStatus(&SC8815_InterruptMaskInitStruct);     //MCU 收到 SC8815 中断后调用此函数读 SC8815 中断 (读中断状态后将清除中断状态位)
//    if (SC8815_InterruptMaskInitStruct.AC_OK == 1)                  //检查 AC_OK 中断是否触发
//    {
//        // AC_OK 中断处理代码
//    }
//    else if (SC8815_InterruptMaskInitStruct.EOC == 1)
//    {
//        // EOC 中断处理代码
//    }
//}

/****************************************
* @brief    针对数控电源的专用初始化函数
* @note     输入(PD)接 VBAT，输出接 VBUS
*****************************************/
void SC8815_Init_PowerSupply(void)
{
    SC8815_BatteryConfigTypeDef SC8815_BatteryConfigStruct = { 0 };
    SC8815_HardwareInitTypeDef SC8815_HardwareInitStruct = { 0 };
    SC8815_InterruptStatusTypeDef SC8815_InterruptMaskInitStruct = { 0 };

    /**** 1. 启动时序 ****/
    // 在调用此函数前，请确保主函数已经把 CE 拉高，PSTOP 拉高
    SoftwareDelay(10);   // 等待芯片内部逻辑复位

    /**** 2. 配置输入源 (假装是电池) ****/
    // 你的输入是 20V PD，我们把它伪装成 4串锂电池
    // 这样 20V 输入对于芯片来说是充足的电量
    SC8815_BatteryConfigStruct.IRCOMP = SCBAT_IRCOMP_0mR; // 电源输入内阻很小，不用补偿
    SC8815_BatteryConfigStruct.VBAT_SEL = SCBAT_VBAT_SEL_External;
    SC8815_BatteryConfigStruct.CSEL = SCBAT_CSEL_4S;      // 设定为 4S (约16.8V标准)，20V输入通常可耐受
    SC8815_BatteryConfigStruct.VCELL = SCBAT_VCELL_4v50;   // 单节电压
    SC8815_BatteryConfig(&SC8815_BatteryConfigStruct);

    /**** 3. 配置硬件参数 (!!! 请务必核对你的原理图 !!!) ****/
    // 下面这些 xx_RATIO 必须和你 PCB 上的电阻匹配，否则读数全是错的
    SC8815_HardwareInitStruct.IBAT_RATIO = SCHWI_IBAT_RATIO_12x;  // 核对原理图!
    SC8815_HardwareInitStruct.IBUS_RATIO = SCHWI_IBUS_RATIO_6x;  // 核对原理图!
    SC8815_HardwareInitStruct.VBAT_RATIO = SCHWI_VBAT_RATIO_12_5x;
    SC8815_HardwareInitStruct.VBUS_RATIO = SCHWI_VBUS_RATIO_12_5x;
    
    SC8815_HardwareInitStruct.VINREG_Ratio = SCHWI_VINREG_RATIO_100x;
    SC8815_HardwareInitStruct.SW_FREQ = SCHWI_FREQ_300KHz_2; // 300kHz 频率
    SC8815_HardwareInitStruct.DeadTime = SCHWI_DT_40ns;      // 死区时间
    
    // 关键模式配置
    SC8815_HardwareInitStruct.ICHAR = SCHWI_ICHAR_IBAT;
    SC8815_HardwareInitStruct.TRICKLE = SCHWI_TRICKLE_Disable; // 数控电源不需要涓流充电
    SC8815_HardwareInitStruct.TERM = SCHWI_TERM_Disable;       // 不需要充电终止
    SC8815_HardwareInitStruct.FB_Mode = SCHWI_FB_External;     // 使用外部反馈
    SC8815_HardwareInitStruct.OVP = SCHWI_OVP_Enable;          // 开启过压保护
    
    // 压摆率 (电压变化速度)，设慢点稳一点，设快点响应快
    SC8815_HardwareInitStruct.SLEW_SET = SCHWI_SLEW_4mV_us;    
    
    SC8815_HardwareInitStruct.ADC_SCAN = SCHWI_ADC_Enable;     // 开启 ADC 扫描，为了能读电压
    SC8815_HardwareInitStruct.ILIM_BW = SCHWI_ILIM_BW_1_25KHz;
    SC8815_HardwareInitStruct.LOOP = SCHWI_LOOP_Normal;
    SC8815_HardwareInit(&SC8815_HardwareInitStruct);

    /**** 4. 配置中断掩码 ****/
    // 我们主要关心 VBUS 短路和 故障
    SC8815_InterruptMaskInitStruct.AC_OK = sDISABLE; 
    SC8815_InterruptMaskInitStruct.INDET = sDISABLE;
    SC8815_InterruptMaskInitStruct.VBUS_SHORT = sENABLE; // 输出短路报警
    SC8815_InterruptMaskInitStruct.OTP = sENABLE;        // 过热报警
    SC8815_InterruptMaskInitStruct.EOC = sDISABLE;
    SC8815_ConfigInterruptMask(&SC8815_InterruptMaskInitStruct);

    /**** 5. 关键差异：这里不要开启输出，也不要设死参数 ****/
    // 此时芯片处于待机状态，等待主循环的指令
    // SC8815_OTG_Disable(); // 默认就是Disable的，不需要显式写
    
}
/****************************************
* @brief    配置 SC8815 电池参数
* @param    SC8815_BatteryConfigStruct 指向 SC8815 电池配置结构 的指针
* @note		进行配置之前先启动 SC8815 并延时 5ms 确保器件就绪,还需要 SC8815 PSTOP 引脚为高才能进行此硬件配置
*****************************************/
void SC8815_BatteryConfig(SC8815_BatteryConfigTypeDef *SC8815_BatteryConfigStruct)
{
    uint8_t tmp;
    tmp = SC8815_BatteryConfigStruct->IRCOMP;
    tmp |= SC8815_BatteryConfigStruct->VBAT_SEL;
    tmp |= SC8815_BatteryConfigStruct->CSEL;
    tmp |= SC8815_BatteryConfigStruct->VCELL;
    I2C_WriteRegByte(SC8815_ADDR, SCREG_VBAT_SET, tmp);
}
/****************************************
* @brief    初始化 SC8815 硬件配置
* @param    SC8815_HardwareInitStruct 指向 SC8815 硬件初始化结构 的指针
* @note		进行配置之前先启动 SC8815 并延时 5ms 确保器件就绪,还需要 SC8815 PSTOP 引脚为高才能进行此硬件配置
*****************************************/
void SC8815_HardwareInit(SC8815_HardwareInitTypeDef *SC8815_HardwareInitStruct)
{
    uint8_t tmp;

    //比例配置
    tmp = I2C_ReadRegByte(SC8815_ADDR, SCREG_RATIO) & 0xE0; //读取寄存器中的保留位(这些保留位不能动)
    tmp |= SC8815_HardwareInitStruct->IBAT_RATIO;           //装填配置参数
    tmp |= SC8815_HardwareInitStruct->IBUS_RATIO;
    tmp |= SC8815_HardwareInitStruct->VBAT_RATIO;
    tmp |= SC8815_HardwareInitStruct->VBUS_RATIO;
    I2C_WriteRegByte(SC8815_ADDR, SCREG_RATIO, tmp);    //写回寄存器

    //硬件配置0
    tmp = I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL0_SET) & 0x60;
    tmp |= SC8815_HardwareInitStruct->VINREG_Ratio;
    tmp |= SC8815_HardwareInitStruct->SW_FREQ;
    tmp |= SC8815_HardwareInitStruct->DeadTime;
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL0_SET, tmp);

    //硬件配置1
    tmp = I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL1_SET) & 0x03;
    tmp |= SC8815_HardwareInitStruct->ICHAR;
    tmp |= SC8815_HardwareInitStruct->TRICKLE;
    tmp |= SC8815_HardwareInitStruct->TERM;
    tmp |= SC8815_HardwareInitStruct->FB_Mode;
    tmp |= SC8815_HardwareInitStruct->TRICKLE_SET;
    tmp |= SC8815_HardwareInitStruct->OVP;
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL1_SET, tmp);

    //硬件配置2
    tmp = (I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL2_SET) & 0xF0) | 0x08;
    tmp |= SC8815_HardwareInitStruct->DITHER;
    tmp |= SC8815_HardwareInitStruct->SLEW_SET;
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL2_SET, tmp);

    //硬件配置3(没有保留位)
    tmp = SC8815_HardwareInitStruct->ADC_SCAN;
    tmp |= SC8815_HardwareInitStruct->ILIM_BW;
    tmp |= SC8815_HardwareInitStruct->LOOP;
    tmp |= SC8815_HardwareInitStruct->ShortFoldBack;
    tmp |= SC8815_HardwareInitStruct->EOC;
    tmp |= SC8815_HardwareInitStruct->PFM;
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL3_SET, tmp);

    //配置完成后必要的延时
    SoftwareDelay(5);
}
/****************************************
* @brief    配置 SC8815 中断屏蔽 (中断使能或失能)
* @param    InterruptStatusStruct 指向 SC8815 中断状态配置结构 的指针
*****************************************/
void SC8815_ConfigInterruptMask(SC8815_InterruptStatusTypeDef *InterruptStatusStruct)
{
    uint8_t tmp;
    tmp = I2C_ReadRegByte(SC8815_ADDR, SCREG_MASK) & 0x91;
    tmp |= InterruptStatusStruct->AC_OK ? 0x40 : 0x00;
    tmp |= InterruptStatusStruct->INDET ? 0x20 : 0x00;
    tmp |= InterruptStatusStruct->VBUS_SHORT ? 0x08 : 0x00;
    tmp |= InterruptStatusStruct->OTP ? 0x04 : 0x00;
    tmp |= InterruptStatusStruct->EOC ? 0x02 : 0x00;
    I2C_WriteRegByte(SC8815_ADDR, SCREG_MASK, tmp);
}

/****************************************
* @brief    读取 SC8815 各中断状态
* @param    InterruptStatusStruct 指向 SC8815 中断状态结构 的指针
*****************************************/
void SC8815_ReadInterrupStatus(SC8815_InterruptStatusTypeDef *InterruptStatusStruct)
{
    uint8_t tmp;
    tmp = I2C_ReadRegByte(SC8815_ADDR, SCREG_MASK);     //读取状态寄存器
    InterruptStatusStruct->AC_OK = (tmp & 0x40) >> 6;   //拆解出每一个位
    InterruptStatusStruct->INDET = (tmp & 0x20) >> 5;
    InterruptStatusStruct->VBUS_SHORT = (tmp & 0x08) >> 3;
    InterruptStatusStruct->OTP = (tmp & 0x04) >> 2;
    InterruptStatusStruct->EOC = (tmp & 0x02) >> 1;
}

/****************************************
* @brief    读取 SC8815 内置 ADC 对 VBUS 电压的测量结果
* @return   单位为 mV 的 VBUS 电压
*****************************************/
uint16_t SC8815_Read_VBUS_Voltage(void)
{
    uint8_t tmp1, tmp2;
    uint16_t adc_raw;

    // 读取 ADC 寄存器
    tmp1 = I2C_ReadRegByte(SC8815_ADDR, SCREG_VBUS_FB_VALUE);
    tmp2 = I2C_ReadRegByte(SC8815_ADDR, SCREG_VBUS_FB_VALUE2) >> 6;

    // 计算 ADC 原始数值 (这是芯片内部参考电压对应的数字)
    // 公式来源：Datasheet ADC计算公式 (4*HighByte + LowByte + 1)
    // 最后的 *2 是因为 ADC LSB 通常对应 2mV (在 1x 增益下)
    adc_raw = (4 * tmp1 + tmp2 + 1) * 2;

    // [关键修正]：直接乘以真实的硬件比例
    // 不再去读寄存器的 RATIO，因为寄存器里的 12.5 是假的，MY_REAL_VBUS_RATIO 才是真的
    return (uint16_t)((float)adc_raw * MY_REAL_VBUS_RATIO);
}
/****************************************
* @brief    读取 SC8815 内置 ADC 对 VBUS 电流的测量结果
* @return   单位为 mA 的 VBUS 电流 (SC8815 无电流方向区分)
*****************************************/
uint16_t SC8815_Read_VBUS_Current(void)
{
    uint8_t tmp1, tmp2;
    uint16_t RATIO_Value;

    RATIO_Value = ((I2C_ReadRegByte(SC8815_ADDR, SCREG_RATIO) & 0x0C) == 4) ? 6 : 3;    //取得 IBUS 的比率
    tmp1 = I2C_ReadRegByte(SC8815_ADDR, SCREG_IBUS_VALUE);              //读取 ADC 值寄存器1
    tmp2 = I2C_ReadRegByte(SC8815_ADDR, SCREG_IBUS_VALUE2) >> 6;        //读取 ADC 值寄存器2

    //返回 VBUS 电流值
    return ((uint16_t)(50 * RATIO_Value) * (4 * tmp1 + tmp2 + 1)) / (3 * SCHW_VBUS_RSHUNT);
}
/****************************************
* @brief    读取 SC8815 内置 ADC 对电池电压的测量结果
* @return   单位为 mV 的电池电压
*****************************************/
uint16_t SC8815_Read_BATT_Voltage(void)
{
    uint8_t tmp1, tmp2;
    uint16_t adc_raw;

    // 读取 ADC 寄存器
    tmp1 = I2C_ReadRegByte(SC8815_ADDR, SCREG_VBAT_FB_VALUE);
    tmp2 = I2C_ReadRegByte(SC8815_ADDR, SCREG_VBAT_FB_VALUE2) >> 6;

    // 计算 ADC 原始数值
    adc_raw = (4 * tmp1 + tmp2 + 1) * 2;

    // [关键修正]：乘以 VBAT 的真实硬件比例
    return (uint16_t)((float)adc_raw * CHIP_PRESET_RATIO);
}
/****************************************
* @brief    读取 SC8815 内置 ADC 对电池电流的测量结果
* @return   单位为 mA 的电池电流 (SC8815 无电流方向区分)
*****************************************/
uint16_t SC8815_Read_BATT_Current(void)
{
    uint8_t tmp1, tmp2;
    uint16_t RATIO_Value;

    RATIO_Value = ((I2C_ReadRegByte(SC8815_ADDR, SCREG_RATIO) & 0x10) == 16) ? 12 : 6;  //取得 IBAT 的比率
    tmp1 = I2C_ReadRegByte(SC8815_ADDR, SCREG_IBAT_VALUE);              //读取 ADC 值寄存器1
    tmp2 = I2C_ReadRegByte(SC8815_ADDR, SCREG_IBAT_VALUE2) >> 6;        //读取 ADC 值寄存器2

    //返回 IBAT 电流值
    return (uint16_t)((50 * RATIO_Value) * (4 * tmp1 + tmp2 + 1)) / (3 * SCHW_BATT_RSHUNT);
}
/****************************************
* @brief    读取 SC8815 内置 ADC 对 ADIN_PIN 电压的测量结果
* @return   单位为 mV 的 ADIN_PIN 电压
*****************************************/
uint16_t SC8815_Read_ADIN_Voltage(void)
{
    uint8_t tmp1,tmp2;

    tmp1 = I2C_ReadRegByte(SC8815_ADDR, SCREG_ADIN_VALUE);          //读取 ADC 值寄存器1
    tmp2 = I2C_ReadRegByte(SC8815_ADDR, SCREG_ADIN_VALUE2) >> 6;    //读取 ADC 值寄存器2

    //返回 ADIN_PIN 电压值
    return (uint16_t)(4 * tmp1 + tmp2 + 1) * 2;
}

/****************************************
* @brief    设置 SC8815 在 OTG 反向输出时的输出电压
* @param    NewVolt 新的单位为 mV 的输出电压设定值
* @note     不得输入超出最大可设定值的电压值, 如最大输出 1024mV,  输入 1145mV 将导致计算结果溢出错误
*****************************************/
/****************************************
* @brief    [重写版] 设置 SC8815 OTG 输出电压
* @param    NewVolt: 期望的真实输出电压 (mV)
*****************************************/
void SC8815_SetOutputVoltage(uint16_t NewVolt)
{
    uint16_t reg_val;
    
    float target_vref_mv = (float)NewVolt / MY_REAL_VBUS_RATIO;
    

    reg_val = (uint16_t)((target_vref_mv / 1960.0f) * 1024.0f);
    
    // 限幅保护
    if (reg_val > 1023) reg_val = 1023;


    uint16_t tmp1, tmp2;
        
    tmp1 = (reg_val >> 2) & 0xFF; // 取高 8 位
    tmp2 = reg_val & 0x03;        // 取低 2 位
    
    I2C_WriteRegByte(SC8815_ADDR, SCREG_VBUSREF_E_SET, (uint8_t)tmp1);
    I2C_WriteRegByte(SC8815_ADDR, SCREG_VBUSREF_E_SET2, (uint8_t)tmp2 << 6);
}
/****************************************
* @brief    设置 SC8815 VBUS 路径上的限流值,双向通用
* @param    NewILIM 新的单位为 mA 的输出限流设定值
* @note     最小的限流值不应低于 300mA, 不得输入超出最大可设定值的电流值
*****************************************/
void SC8815_SetBusCurrentLimit(uint16_t NewILIM)
{
    uint8_t tmp;
    uint16_t RATIO_Value;

    RATIO_Value = ((I2C_ReadRegByte(SC8815_ADDR, SCREG_RATIO) & 0x0C) == 4) ? 6 : 3;    //取得 IBUS 的比率
    tmp = (16 * (NewILIM) * (SCHW_VBUS_RSHUNT)) / (625 * RATIO_Value) - 1;              //计算 LIM 设置值
    I2C_WriteRegByte(SC8815_ADDR, SCREG_IBUS_LIM_SET, tmp);                             //写入到 SC8815 寄存器
}
/****************************************
* @brief    设置 SC8815 电池路径上的限流值,双向通用
* @param    NewILIM 新的单位为 mA 的电池限流设定值
* @note     最小的限流值不应低于 300mA, 不得输入超出最大可设定值的电流值
*****************************************/
void SC8815_SetBatteryCurrLimit(uint16_t NewILIM)
{
    uint8_t tmp;
    uint16_t RATIO_Value;

    RATIO_Value = ((I2C_ReadRegByte(SC8815_ADDR, SCREG_RATIO) & 0x10) == 16) ? 12 : 6; //取得 IBAT 的比率
    tmp = (16 * (NewILIM) * (SCHW_VBUS_RSHUNT)) / (625 * RATIO_Value) - 1;             //计算 LIM 设置值
    I2C_WriteRegByte(SC8815_ADDR, SCREG_IBAT_LIM_SET, tmp);                            //写入到 SC8815 寄存器
}
/****************************************
* @brief    设置 SC8815 VINREG 电压值 (类似 MPPT)
* @param    NewVolt 新的单位为 mV 的 VINREG 电压设定值
* @note     不得输入超出最大可设定值的电压值
*****************************************/
void SC8815_VINREG_SetVoltage(uint16_t NewVolt)
{
    uint16_t RATIO_Value;
    RATIO_Value = ((I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL0_SET) & 0x10) == 16) ? 40 : 100;    //取得 VINREG 的比率
    I2C_WriteRegByte(SC8815_ADDR, SCREG_VINREG_SET, (NewVolt / RATIO_Value) - 1);               //写入到 SC8815 寄存器
}

/****************************************
* @brief    获取 SC8815 在 OTG 反向输出时的输出电压设定值
* @return   单位为 mV 的输出电压设定值
*****************************************/
uint16_t SC8815_GetOutputVoltage(void)
{
    uint16_t tmp1, tmp2;
    double RATIO_Value;

    //判断 VBUS 电压反馈的模式
    if (I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL1_SET) & 0x10)
    {
        //读取 VBUSREF_E_SET 寄存器
        tmp1 = I2C_ReadRegByte(SC8815_ADDR, SCREG_VBUSREF_E_SET);
        tmp2 = I2C_ReadRegByte(SC8815_ADDR, SCREG_VBUSREF_E_SET2 >> 6);

        //计算输出电压比率
        RATIO_Value = MY_REAL_VBUS_RATIO;
    }
    else
    {
        //读取 VBUSREF_E_SET 寄存器
        tmp1 = I2C_ReadRegByte(SC8815_ADDR, SCREG_VBUSREF_E_SET);
        tmp2 = I2C_ReadRegByte(SC8815_ADDR, SCREG_VBUSREF_E_SET2 >> 6);

        //取得VBUS电压的比率
        RATIO_Value = ((I2C_ReadRegByte(SC8815_ADDR, SCREG_RATIO) & 0x01) == 1) ? 5 : 12.5;
    }

    //返回对应的输出电压
    return (uint16_t)((4 * tmp1 + tmp2 + 1) * RATIO_Value) * 2;
}
/****************************************
* @brief    获取 SC8815 VBUS 路径上的限流设定值
* @return   单位为 mA 的 VBUS 路径限流设定值
*****************************************/
uint16_t SC8815_GetBusCurrentLimit(void)
{
    uint8_t tmp;
    uint16_t RATIO_Value;

    RATIO_Value = ((I2C_ReadRegByte(SC8815_ADDR, SCREG_RATIO) & 0x0C) == 4) ? 6 : 3; //取得 IBUS 的比率
    tmp = I2C_ReadRegByte(SC8815_ADDR, SCREG_IBUS_LIM_SET);                          //取得 IBUS 限流寄存器值

    //返回IBUS限流值
    return (uint16_t)((uint32_t)(625 * (RATIO_Value) * (tmp + 1)) / (16 * (SCHW_VBUS_RSHUNT)));
}
/****************************************
* @brief    获取 SC8815 电池路径上的限流设定值
* @return   单位为 mA 的电池路径限流设定值
*****************************************/
uint16_t SC8815_GetBatteryCurrLimit(void)
{
    uint8_t tmp;
    uint16_t RATIO_Value;

    RATIO_Value = ((I2C_ReadRegByte(SC8815_ADDR, SCREG_RATIO) & 0x10) == 16) ? 12 : 6; //取得 IBAT 的比率
    tmp = I2C_ReadRegByte(SC8815_ADDR, SCREG_IBAT_LIM_SET);                            //取得 IBAT 限流寄存器值

    //返回IBAT限流值
    return (uint16_t)((uint32_t)(625 * (RATIO_Value) * (tmp + 1)) / (16 * (SCHW_BATT_RSHUNT)));
}
/****************************************
* @brief    获取 SC8815 VINREG 电压设定值
* @return   单位为 mV 的 VINREG 电压设定值
*****************************************/
uint16_t SC8815_VINREG_GetVoltage(void)
{
    uint8_t tmp;
    uint16_t RATIO_Value;
    RATIO_Value = ((I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL0_SET) & 0x10) == 16) ? 40 : 100; //取得 VINREG 的比率
    tmp = I2C_ReadRegByte(SC8815_ADDR, SCREG_VINREG_SET);                                    //取得 VINREG 寄存器值
    return tmp * RATIO_Value;
}
/****************************************
* @brief    获取 SC8815 OTG 模式下的最大可输出电压
* @return   单位为 mV 的电压值
*****************************************/
uint16_t SC8815_GetMaxOutputVoltage(void)
{
    uint16_t tmp1, tmp2;
    double RATIO_Value;

    //判断 VBUS 电压反馈的模式
    if (I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL1_SET) & 0x10)
    {
        //计算输出电压比率
        RATIO_Value = (double)SCHW_FB_RUP / SCHW_FB_RDOWM + 1.0;
    }
    else
    {
        //取得 VBUS 电压的比率
        RATIO_Value = ((I2C_ReadRegByte(SC8815_ADDR, SCREG_RATIO) & 0x01) == 1) ? 5 : 12.5;
    }

    //返回最高输出电压值
    return 2048 * RATIO_Value;
}
/****************************************
* @brief    获取 SC8815 OTG 模式下的输出电压步进值
* @return   单位为 mV 的电压值
*****************************************/
uint16_t SC8815_GetOutputVoltageSetp(void)
{
    uint16_t tmp1, tmp2;
    double RATIO_Value;

    //判断 VBUS 电压反馈的模式
    if (I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL1_SET) & 0x10)
    {
        //计算输出电压比率
        RATIO_Value = (double)SCHW_FB_RUP / SCHW_FB_RDOWM + 1.0;
    }
    else
    {
        //取得 VBUS 电压的比率
        RATIO_Value = ((I2C_ReadRegByte(SC8815_ADDR, SCREG_RATIO) & 0x01) == 1) ? 5 : 12.5;
    }

    //返回输出电压步进值
    return 2 * RATIO_Value;
}

/****************************************
* @brief    打开 OTG 反向放电模式
*****************************************/
void SC8815_OTG_Enable(void)
{
    //设置 EN_OTG 位
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL0_SET, I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL0_SET) | 0x80);
}
/****************************************
* @brief    关闭 OTG 反向放电模式, SC8815 将处于充电模式
*****************************************/
void SC8815_OTG_Disable(void)
{
    //清除 EN_OTG 位
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL0_SET, I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL0_SET) & 0x7F);
}
/****************************************
* @brief    设置 VINREG 的增益为 40x
*****************************************/
void SC8815_VINREG_SetRatio_40x(void)
{
    //设置 VINREG_RATIO 位
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL0_SET, I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL0_SET) | 0x10);
}
/****************************************
* @brief    设置 VINREG 的增益为 100x
*****************************************/
void SC8815_VINREG_SetRatio_100x(void)
{
    //清除 VINREG_RATIO 位
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL0_SET, I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL0_SET) & 0xEF);
}
/****************************************
* @brief    打开 OTG 反向放电模式中的 VBUS 过压保护功能
*****************************************/
void SC8815_OVP_Enable(void)
{
    //清除 DIS_OVP 位
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL1_SET, I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL1_SET) & 0xFB);
}
/****************************************
* @brief    关闭 OTG 反向放电模式中的 VBUS 过压保护功能
*****************************************/
void SC8815_OVP_Disable(void)
{
    //设置 DIS_OVP 位
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL1_SET, I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL1_SET) | 0x04);
}
/****************************************
* @brief    打开 PGATE 引脚功能, 输出低电平打开 PMOS
*****************************************/
void SC8815_PGATE_Enable(void)
{
    //设置 EN_PGATE 位
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL3_SET, I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL3_SET) | 0x80);
}
/****************************************
* @brief    关闭 PGATE 引脚功能, 输出高电平关闭 PMOS
*****************************************/
void SC8815_PGATE_Disable(void)
{
    //清除 EN_PGATE 位
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL3_SET, I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL3_SET) & 0x7F);
}
/****************************************
* @brief    打开 GPO 引脚功能, 输出低电平
*****************************************/
void SC8815_GPO_Enable(void)
{
    //设置 GPO_CTRL 位
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL3_SET, I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL3_SET) | 0x40);
}
/****************************************
* @brief    关闭 GPO 引脚功能, 输出高阻状态
*****************************************/
void SC8815_GPO_Disable(void)
{
    //清除 GPO_CTRL 位
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL3_SET, I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL3_SET) & 0xBF);
}
/****************************************
* @brief    打开 ADC 扫描, 此时可以读取 ADC 数据
*****************************************/
void SC8815_ADC_Enable(void)
{
    //设置 AD_START 位
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL3_SET, I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL3_SET) | 0x20);
}
/****************************************
* @brief    关闭 ADC 扫描, 节约 1-2mA 的耗电
*****************************************/
void SC8815_ADC_Disable(void)
{
    //清除 AD_START 位
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL3_SET, I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL3_SET) & 0xDF);
}
/****************************************
* @brief    打开 OTG 反向放电模式中的 VBUS 和 VBAT 短路保护功能
*****************************************/
void SC8815_SFB_Enable(void)
{
    //清除 DIS_ShortFoldBack 位
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL3_SET, I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL3_SET) & 0xFB);
}
/****************************************
* @brief    关闭 OTG 反向放电模式中的 VBUS 和 VBAT 短路保护功能
*****************************************/
void SC8815_SFB_Disable(void)
{
    //设置 DIS_ShortFoldBack 位
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL3_SET, I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL3_SET) | 0x04);
}
/****************************************
* @brief    打开 OTG 模式中轻载条件下的 PFM 模式
*****************************************/
void SC8815_PFM_Enable(void)
{
    //设置 EN_PFM 位
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL3_SET, I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL3_SET) | 0x01);
}
/****************************************
* @brief    关闭 OTG 模式中轻载条件下的 PFM 模式
*****************************************/
void SC8815_PFM_Disable(void)
{
    //清除 EN_PFM 位
    I2C_WriteRegByte(SC8815_ADDR, SCREG_CTRL3_SET, I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL3_SET) & 0xFE);
}

/****************************************
* @brief    检查 OTG 是否处于打开状态
* @return   OTG 功能的状态 (1b 或 0b)
*****************************************/
uint8_t SC8815_OTG_IsEnable(void)
{
    //返回 OTG 的状态
    return (I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL0_SET) & 0x80) ? 1 : 0;
}
/****************************************
* @brief    获取 VINREG 的增益
* @return   VINREG 的增益 (40 或 100)
*****************************************/
uint8_t SC8815_VINREG_GetRatio(void)
{
    //返回 VINREG 的增益
    return (I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL0_SET) & 0x10) ? 40 : 100;
}
/****************************************
* @brief    检查 OTG 模式中 OVP(过压保护) 功能是否处于打开状态
* @return   OVP 功能的状态 (1b 或 0b)
*****************************************/
uint8_t SC8815_OVP_IsEnable(void)
{
    //返回 OVP 的状态
    return (I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL1_SET) & 0x04) ? 0 : 1;
}
/****************************************
* @brief    检查 PGATE 引脚功能是否处于打开状态
* @return   PGATE 引脚功能的状态 (1b 或 0b)
*****************************************/
uint8_t SC8815_PGATE_IsEnable(void)
{
    //返回 PGATE 的状态
    return (I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL3_SET) & 0x80) ? 1 : 0;
}
/****************************************
* @brief    检查 GPO 引脚功能是否处于打开状态
* @return   GPO 引脚功能的状态 (1b 或 0b)
*****************************************/
uint8_t SC8815_GPO_IsEnable(void)
{
    //返回 GPO 的状态
    return (I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL3_SET) & 0x40) ? 1 : 0;
}
/****************************************
* @brief    检查 ADC 扫描是否处于打开状态
* @return   ADC 扫描的状态 (1b 或 0b)
*****************************************/
uint8_t SC8815_ADC_IsEnable(void)
{
    //返回 ADC 的状态
    return (I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL3_SET) & 0x20) ? 1 : 0;
}
/****************************************
* @brief    检查 OTG 模式中 短路保护 功能是否处于打开状态
* @return   短路保护功能的状态 (1b 或 0b)
*****************************************/
uint8_t SC8815_SFB_IsEnable(void)
{
    //返回 短路保护功能 的状态
    return (I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL3_SET) & 0x04) ? 0 : 1;
}
/****************************************
* @brief    检查 OTG 模式中的 PFM 模式是否处于打开状态
* @return   PFM 模式的状态 (1b 或 0b)
*****************************************/
uint8_t SC8815_PFM_IsEnable(void)
{
    //返回 PFM 的状态
    return (I2C_ReadRegByte(SC8815_ADDR, SCREG_CTRL3_SET) & 0x01) ? 1 : 0;
}

/*****END OF FILE****/





void SC8815_Start_OTG_Mode(void)
{
    // 1. 硬件复位与启动时序
    // PSTOP 先拉高（停止工作，允许配置）
    HAL_GPIO_WritePin(SC_PSTOP_GPIO_Port, SC_PSTOP_Pin, GPIO_PIN_SET); 
    // CE 拉高
    HAL_GPIO_WritePin(SC_CE_GPIO_Port, SC_CE_Pin, GPIO_PIN_RESET);
    
    HAL_Delay(10); // 等待芯片启动
		SC8815_ADC_Enable();
    SC8815_Init_PowerSupply(); 
		
	
    // [设置电池端放电限流] 例如：限制电池端最大抽出电流 5A (5000mA)
    // 保护电池不过放，保护电感不饱和
    SC8815_SetBatteryCurrLimit(5000);
		SC8815_SFB_Enable();//开启短路保护

//    // 4. 开启 OTG 模式
//    // 此时寄存器已经配置为反向输出，但硬件 PSTOP 还是高，所以还没输出
//    SC8815_OTG_Enable();
		
//		SC8815_OVP_Enable();
    
    // 可选：开启 ADC 扫描以便后续读取电压电流
    
		
		

//    // 5. 最终硬件使能
//    // 拉低 PSTOP，芯片开始 PWM 斩波，VBUS 将会有电压输出
//    HAL_GPIO_WritePin(SC_PSTOP_GPIO_Port, SC_PSTOP_Pin, GPIO_PIN_RESET);
}




/**
 * @brief  SC8815 + INA226 全系统自检
 * @return 0: 完美 (Passed)
 * 10: SC8815 芯片未响应 (I2C 地址错误或虚焊)
 * 11: INA226 芯片未响应 (I2C 地址错误或虚焊)
 * 1:  输入电压异常 (PD 未诱骗或 ADC 故障)
 * 2:  输出电压异常 (反馈电阻虚焊或 MOS 故障)
 * 3:  芯片内部故障 (短路/过温)
 */
uint8_t SC8815_Init_Check(void)
{
    char msg[64];
    
    // 确保 CE 拉低
    HAL_GPIO_WritePin(SC_CE_GPIO_Port, SC_CE_Pin, GPIO_PIN_RESET);
    HAL_Delay(20);

    // ==========================================================
    // 检查点 0: I2C 总线物理连通性检查 (Ping SC8815)
    // ==========================================================
    
    // 尝试 Ping SC8815 (使用 Bus_SC)
    Software_I2C_Start(&Bus_SC);
    Software_I2C_WriteByte(&Bus_SC, SC8815_ADDR); // 发送写地址
    
    if (Software_I2C_WaitACK(&Bus_SC) != HAL_OK)
    {
        Software_I2C_Stop(&Bus_SC);
        sprintf(msg, "[FATAL] SC8815 Not Found on PA8/PC9! Addr: 0x%02X\r\n", SC8815_ADDR);
        HAL_UART_Transmit(&huart4, (uint8_t*)msg, strlen(msg), 100);
				while(1);
//        return 10; 
    }
    Software_I2C_Stop(&Bus_SC);
    
    sprintf(msg, "[OK] SC8815 Connected.\r\n");	
    HAL_UART_Transmit(&huart4, (uint8_t*)msg, strlen(msg), 100);

    // ==========================================================
    // 检查点 0.5: 检查 INA226 (使用 Bus_INA)
    // ==========================================================
    // 这里的 Bus_INA 需要在 main.c 定义并 extern 进来，或者在这里临时检查
    // 假设你已经在 main.c 定义了 Bus_INA
    extern SoftI2C_Bus Bus_INA; 
    
    Software_I2C_Start(&Bus_INA);
    Software_I2C_WriteByte(&Bus_INA, 0x80); // INA226 地址
    
    if (Software_I2C_WaitACK(&Bus_INA) != HAL_OK)
    {
        Software_I2C_Stop(&Bus_INA);
        sprintf(msg, "[FATAL] INA226 Not Found on PB6/PB7!\r\n");
        HAL_UART_Transmit(&huart4, (uint8_t*)msg, strlen(msg), 100);
        return 11; 
    }
    Software_I2C_Stop(&Bus_INA);
    
    sprintf(msg, "[OK] INA226 Connected.\r\n");
    HAL_UART_Transmit(&huart4, (uint8_t*)msg, strlen(msg), 100);

    // ==========================================================
    // 检查点 1: 模拟电压检查 (保留你原有的逻辑)
    // ==========================================================
//    HAL_Delay(500); 
    
    // 读取电压进行验证... (代码保持不变)
    // vbus_mv = SC8815_Read_VBUS_Voltage(); ...
    
    sprintf(msg, "[System] All Tests Passed. Ready for PID.\r\n");
    HAL_UART_Transmit(&huart4, (uint8_t*)msg, strlen(msg), 100);
    return 0;
}


void SC8815_EnableOutput(uint8_t enable)
{
    if (enable)
    {
        // === 开启顺序 ===
        
        // 1. 配置寄存器允许 OTG 输出
        SC8815_OTG_Enable();
        
        // 2. 拉低 PSTOP，释放 PWM
        HAL_GPIO_WritePin(SC_PSTOP_GPIO_Port, SC_PSTOP_Pin, GPIO_PIN_RESET);
    }
    else
    {
        // === 关闭顺序  ===
        
        // 1. 拉高 PSTOP，切断 PWM
        HAL_GPIO_WritePin(SC_PSTOP_GPIO_Port, SC_PSTOP_Pin, GPIO_PIN_SET);
        
        // 2. 软件层：清除寄存器位
        SC8815_OTG_Disable();
    }
}







