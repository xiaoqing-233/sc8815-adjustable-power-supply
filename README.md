# SC8815 可调电源

基于 STM32F405 的数控可调电源工程，采用 SC8815 作为功率控制芯片，配合 LCD 界面、INA226 电流检测和 PID 温度控制，实现电源参数显示与调节。

## 功能
- 输出电压/电流调节
- LCD 图形界面显示
- INA226 电压、电流检测
- NTC 温度采样与 PID 控制
- Keil MDK 工程支持

## 工程结构
- `f405 _shukon/Core`：STM32 核心代码
- `f405 _shukon/USER_code`：用户功能代码
- `f405 _shukon/MDK-ARM`：Keil 工程文件
- `f405 _shukon/lvgl`：LVGL 图形库
- `外壳`：外壳 3D 模型文件

## 开发环境
- MCU：STM32F405RGT6
- 开发工具：Keil MDK
- 图形库：LVGL
- 语言：C

## 编译
使用 Keil MDK 打开 `f405 _shukon/MDK-ARM/f405 _shukon.uvprojx` 进行编译。

## 说明
如果你想把这个项目继续整理成更标准的 GitHub 仓库，我可以继续帮你补充：
- 中文/英文双语 README
- `LICENSE`
- `.gitignore`
- 目录说明和接线说明
