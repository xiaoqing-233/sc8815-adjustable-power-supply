# SC8815 可调电源

这是一个基于 `STM32F405` 的数控可调电源工程，核心功率芯片为 `SC8815`，配合 `INA226` 电压电流检测、`NTC` 温度采样、`PID` 控制和 `LCD + LVGL` 图形界面，实现电源参数显示、调节与状态控制。

仓库中包含完整固件工程、界面资源、第三方图形库以及外壳模型文件，适合继续开发、复现硬件功能，或作为同类电源项目的参考。

## 项目特点

- 基于 `STM32F405RGT6` 主控
- 使用 `SC8815` 进行电源控制
- 使用 `INA226` 采集总线电压、电流、功率信息
- 使用 `NTC` 进行温度检测
- 使用 `PID` 实现输出控制逻辑
- 使用 `LVGL 8.3.11` 构建图形界面
- 使用 `SquareLine Studio` 生成部分 UI 代码
- 使用 `Keil MDK` 作为主要工程环境

## 主要功能

- 输出电压设定
- 输出电流限流设定
- 电压、电流、功率实时显示
- 温度采样与显示
- 图形界面交互
- 输出开关控制
- 电源状态刷新与事件响应

## 硬件组成

从当前工程代码可以确认的主要硬件如下：

- MCU：`STM32F405RGT6`
- 电源控制芯片：`SC8815`
- 电流/功率检测：`INA226`
- 显示屏：SPI 接口 LCD，分辨率 `320x240`
- 图形库：`LVGL`
- 温度检测：`100K NTC`

代码中还可以看到按键、蜂鸣器、屏幕控制引脚等外设定义，适合做本地独立交互控制。

## 软件架构

工程主要由以下几个部分组成：

- `STM32 HAL`：底层外设驱动与初始化
- `USER_code`：电源控制、采样、温控、软件 I2C 等用户逻辑
- `LCD`：显示驱动、GUI 逻辑与 SquareLine 生成的界面代码
- `LVGL`：图形库本体
- `CubeMX/Keil`：工程配置与构建入口

## 目录结构

根目录：

- `README.md`：项目说明文档
- `可调电源.eprj`：工程相关文件
- `外壳/`：外壳 3D 模型文件
- `f405 _shukon/`：主固件工程目录

主工程目录 `f405 _shukon/`：

- `Core/Inc`：主控头文件、引脚定义、外设声明
- `Core/Src`：主程序入口与 CubeMX 生成的核心代码
- `Drivers/`：STM32 HAL 驱动与 CMSIS 相关代码
- `USER_code/`：用户功能代码
- `LCD/`：LCD 驱动、界面逻辑、SquareLine UI 资源
- `lvgl/`：LVGL 第三方图形库源码
- `MDK-ARM/`：Keil MDK 工程文件
- `.mxproject`：CubeMX 工程配置文件
- `f405 _shukon.ioc`：STM32CubeMX 工程文件

## 重点源码位置

如果你要继续开发，建议优先看这些文件：

- `f405 _shukon/Core/Src/main.c`
  - 系统初始化入口
  - LVGL 初始化
  - 外设启动
  - SC8815 / INA226 初始化

- `f405 _shukon/USER_code/SC8815.c`
  - SC8815 配置与控制逻辑

- `f405 _shukon/USER_code/INA226_Driver.c`
  - INA226 采样驱动

- `f405 _shukon/USER_code/PID.c`
  - 电压/电流调节相关 PID 控制

- `f405 _shukon/USER_code/ntc_temp.c`
  - NTC 温度采样与换算

- `f405 _shukon/LCD/ui.c`
  - 图形界面入口

- `f405 _shukon/LCD/GUI.c`
  - 界面业务逻辑

## 构建方式

### Keil MDK

直接打开以下工程文件：

`f405 _shukon/MDK-ARM/f405 _shukon.uvprojx`

然后在 Keil 中编译、下载即可。

### CubeMX

如果需要重新生成外设初始化代码，可以打开：

`f405 _shukon/f405 _shukon.ioc`

重新配置后再生成工程代码。
