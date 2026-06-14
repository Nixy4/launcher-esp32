# LILYGO T-LoraPager 开发参考文档

> 来源：<https://wiki.lilygo.cc/zh/products/t-lora-series/t-lora-pager/>
> 采集日期：2026-06-14

---

## 概述

T-LoraPager 是一款 LILYGO 推出的手持式 AIoT 可编程开发设备，集成了 ESP32-S3 高性能 Wi-Fi/蓝牙双模芯片与多种无线通信模块。设备采用小巧外观与可折叠外部天线设计。提供多种版本选择，主要区别在于 LoRa 模块（LR1121、SX1262 或 CC1101）。

---

## 产品参数

| 参数 | 值 |
|------|-----|
| MCU | ESP32-S3 |
| Flash | 16 MB |
| PSRAM | 8 MB |
| 屏幕 | 2.33 英寸 ST7796 IPS LCD（480×222） |
| LoRa | SX1262（433–920 MHz），可选 LR1121 / CC1101 |
| GPS | MIA-M10Q（U-blox） |
| NFC | ST25R3916（SPI） |
| AI 传感器 | BHI260AP |
| 音频 | ES8311（麦克风 + 扬声器 + 耳机） |
| RTC | PCF85063A |
| 充电芯片 | BQ25896 |
| 电量监测 | BQ27220 |
| 振动电机 | DRV2605 |
| 存储 | TF 卡（最大 32 GB FAT32） |
| 无线 | 2.4 GHz Wi-Fi & Bluetooth 5（LE） |
| USB | 1 × TYPE-C |
| 输入 | QWERT 键盘（TCA8418）+ 旋转编码器 |
| IO 扩展 | XL9555（I²C） |
| 扩展接口 | GPS 接口 + 2 × Knockout + 2.54 mm 2×8 GPIO |
| 按键 | RESET + BOOT |
| 固定孔 | 1/4 英寸螺丝接口 + 4 × M2 背孔 |
| 尺寸 | 106 × 89 × 23 mm |

---

## 主要芯片列表

| 芯片 | 功能 |
|------|------|
| ESP32-S3 | 主控 MCU |
| ST7796 | LCD 驱动 |
| SX1262 / LR1121 / CC1101 | LoRa 无线模块 |
| MIA-M10Q | U-blox GPS |
| ST25R3916 | NFC/RFID（SPI） |
| BHI260AP | AI IMU 传感器 |
| ES8311 | 音频编解码器 |
| PCF85063A | RTC |
| BQ25896 | 锂电池充电管理 |
| BQ27220 | 电量计 |
| DRV2605 | 振动马达驱动 |
| TCA8418 | QWERT 键盘矩阵 |
| XL9555 | I²C GPIO 扩展 |

---

## 引脚映射（GPIO）

| 功能 | GPIO | 可用于用户程序 |
|------|------|--------------|
| Custom Pin（外部12-Pin接口） | 9 | ✅ |
| UART1 TX（外部12-Pin接口） | 43 | ✅ |
| UART1 RX（外部12-Pin接口） | 44 | ✅ |
| SDA（I²C） | 3 | ❌ |
| SCL（I²C） | 2 | ❌ |
| SPI MOSI | 34 | ❌ |
| SPI MISO | 33 | ❌ |
| SPI SCK | 35 | ❌ |
| SD CS | 21 | ❌ |
| Keyboard INT | 6 | ❌ |
| Keyboard BL | 46 | ❌ |
| Rotary Encoder A | 40 | ❌ |
| Rotary Encoder B | 41 | ❌ |
| Rotary Encoder SW | 7 | ❌ |
| RTC INT | 1 | ❌ |
| NFC CS | 39 | ❌ |
| NFC INT | 5 | ❌ |
| Sensor INT（BHI260AP） | 8 | ❌ |
| Audio WS（I²S） | 18 | ❌ |
| Audio SCK（I²S） | 11 | ❌ |
| Audio MCLK（I²S） | 10 | ❌ |
| Audio Dout（I²S） | 45 | ❌ |
| Audio Din（I²S） | 17 | ❌ |
| GNSS TX | 12 | ❌ |
| GNSS RX | 4 | ❌ |
| GNSS PPS | 13 | ❌ |
| LoRa RST | 47 | ❌ |
| LoRa BUSY | 48 | ❌ |
| LoRa CS | 36 | ❌ |
| LoRa INT | 14 | ❌ |
| Display CS | 38 | ❌ |
| Display DC | 37 | ❌ |
| Display BL | 42 | ❌ |

> ✅ = 可在用户程序中使用；❌ = 已被板载外设占用，勿复用。

---

## 软件开发

### 主库

- **LilyGoLib**：<https://github.com/Xinyuan-LilyGO/LilyGoLib>
- **LilyGoLib-ThirdParty**：<https://github.com/Xinyuan-LilyGO/LilyGoLib-ThirdParty>

### 开发平台

1. [PlatformIO](https://platformio.org/)
2. [Arduino IDE](https://www.arduino.cc/en/software)
3. [VS Code](https://code.visualstudio.com/)
4. [MicroPython](https://micropython.org/)

---

## 快速开始

### PlatformIO

1. 安装 VS Code，安装 "PlatformIO IDE" 扩展。
2. 下载 [LilyGoLib 项目代码](https://github.com/Xinyuan-LilyGO/LilyGoLib)。
3. 在 VS Code 中打开项目，编辑 `platformio.ini` 选择目标环境。
4. 连接设备，编译并烧录。

### Arduino IDE

1. 安装 [Arduino IDE](https://www.arduino.cc/en/software)。
2. 安装 **Arduino ESP32 V3.3.0-alpha1 或更高版本**：
   - 管理器 URL：`https://espressif.github.io/arduino-esp32/package_esp32_dev_index.json`
3. 下载并添加 [LilyGoLib 库](https://github.com/Xinyuan-LilyGO/LilyGoLib/archive/refs/heads/master.zip)（`.ZIP` 方式导入）。
4. 将 [LilyGoLib-ThirdParty](https://github.com/Xinyuan-LilyGO/LilyGoLib-ThirdParty) 所有目录复制到 Arduino 库目录。
5. 打开示例：`文件 → 示例 → LilyGOLib → helloworld`

#### Arduino IDE 开发板配置

| 选项 | 值 |
|------|-----|
| Board | LilyGo-T-LoRa-Pager |
| Upload Speed | 921600 |
| USB Mode | CDC and JTAG |
| USB CDC On Boot | **Enabled** |
| USB Firmware MSC On Boot | Disabled |
| USB DFU On Boot | Disabled |
| CPU Frequency | 240 MHz (WiFi) |
| Core Debug Level | None |
| Partition Scheme | 16M Flash (3MB APP/9.9MB FATFS) |
| Board Revision | 根据购买的射频版本选择（见下） |
| Upload Mode | UART0/Hardware CDC |
| Arduino Runs On | Core 1 |
| Events Run On | Core 1 |

#### 射频版本选择（Board Revision）

| 选项 | 对应模块 |
|------|---------|
| Radio-SX1262 | Sub-1G LoRa |
| Radio-SX1280 | 2.4G LoRa |
| Radio-CC1101 | Sub-1G (G)MSK / FSK / ASK / OOK |
| Radio-LR1121 | Sub-1G + 2.4G LoRa（多频段） |
| Radio-SI4432 | Sub-1G ISM |

> **注意**：若串口无输出，请确认 `USB CDC On Boot` 已设置为 `Enabled`。

---

## 示例程序支持

| 示例 | 说明 |
|------|------|
| UI Demo | 出厂 UI 演示程序 |
| LoRa Example | LoRa 通信示例 |
| GPS Example | GPS 定位示例 |
| Keyboard Example | 键盘输入示例 |
| Audio Example | 音频播放示例 |

---

## 功耗参考

| 模式 | 唤醒方式 | 电流 |
|------|---------|------|
| Deep Sleep | BOOT 按键 | ~530 µA |
| Deep Sleep | 定时器 | ~530 µA |
| Light Sleep | BOOT 按键 | ~2.26 mA |
| Power OFF | 电源按键 | ~26 µA |

---

## 常见问题

- **无法烧录**：确保 `USB CDC On Boot` 已启用，按住 BOOT 键再按 RESET 进入下载模式。
- **串口无输出**：检查 `USB CDC On Boot` 是否为 `Enabled`。
- **GPS 定位慢/无信号**：在户外开阔区域使用，并检查天线连接。
- **Arduino 报错**：需要 arduino-esp32 **V3.3.0-alpha1 或更高版本**。

---

## 参考链接

| 资源 | 链接 |
|------|------|
| 官方 Wiki | <https://wiki.lilygo.cc/zh/products/t-lora-series/t-lora-pager/> |
| GitHub 主库 | <https://github.com/Xinyuan-LilyGO/LilyGoLib> |
| 原理图 PDF | <https://github.com/Xinyuan-LilyGO/LilyGoLib/blob/master/Files/(N314)T-Lora_Pager_LR1121_Module_V1.0_20250805.pdf> |
| 出厂固件说明 | <https://wiki.lilygo.cc/zh/products/t-lora-series/t-lora-pager/factory.html> |
| ESP32-S3 数据手册 | <https://www.espressif.com.cn/sites/default/files/documentation/esp32-s3_datasheet_en.pdf> |
| SX1262 数据手册 | <https://www.semtech.com/products/wireless-rf/lora-core/sx1262> |
| MIA-M10Q 数据手册 | <https://www.u-blox.com/en/product/mia-m10-series> |
| BHI260AP 数据手册 | <https://www.bosch-sensortec.com/products/motion-sensors/imu/bhi260ap/> |

---

## 版本历史

| 版本 | 日期 | 说明 |
|------|------|------|
| T-LoraPager_V1.0 | 2024-08-05 | 初始版本 |
