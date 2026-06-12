# Plan: wrapper-esp32 SD卡/文件系统新模块实现

## TL;DR
在 `launcher-esp32/components/wrapper-esp32/src/wrapper/` 下新增 4 个模块（8 个文件），遵循现有 wrapper 风格（Config struct 继承 IDF struct、Logger& 构造、Init/Deinit 对），覆盖 SPI 模式 SD 卡、SDMMC 模式 SD 卡、Flash FAT（Wear-Levelling）、SPIFFS 四类存储接口。目标 IDF 版本：6.0.1，目标芯片：ESP32-P4。

---

## 参考基准（现有 wrapper 约定）
- Config struct 继承 IDF struct，构造函数设置所有字段（参考 `SpiBusConfig`、`LedcTimerConfig`）
- Class 持有 `Logger& logger_`、handle（如 `wl_handle_t`/`sdmmc_card_t*`）、`bool initialized_`
- Init/Deinit 对，Deinit 在析构中调用
- 所有代码在 `namespace wrapper`
- `.hpp` 用 `#pragma once`，`.cpp` 不需要 guard

---

## 新增文件（8个）

| 文件 | 对应 IDF 接口 | VFS 挂载路径 |
|---|---|---|
| `sd_spi.hpp/.cpp` | `esp_vfs_fat_sdspi_mount` | `/sdcard` |
| `sd_mmc.hpp/.cpp` | `esp_vfs_fat_sdmmc_mount` (legacy) | `/sdcard` |
| `fatfs.hpp/.cpp` | `esp_vfs_fat_spiflash_mount_rw_wl` | `/ffat` |
| `spiffs.hpp/.cpp` | `esp_vfs_spiffs_register` | `/spiffs` |

---

## sd_spi

### SdSpiMountConfig : public esp_vfs_fat_mount_config_t
构造参数：`format_if_mount_failed=false, max_files=5, allocation_unit_size=0, disk_status_check_enable=false`

### SdSpiDevConfig : public sdspi_device_config_t
构造参数：`gpio_cs, gpio_cd=NC, gpio_wp=NC`
host_id 由 `Init()` 从 `SpiBus::GetHostId()` 注入

### class SdSpi
成员：`Logger& logger_`, `sdmmc_card_t* card_`, `bool mounted_`, `std::string base_path_`

```cpp
bool Init(const SpiBus& spi_bus, const SdSpiDevConfig&,
          const SdSpiMountConfig&, const char* base_path="/sdcard");
bool Deinit();
bool IsMounted() const;
const char* GetBasePath() const;
sdmmc_card_t* GetCard() const;
void PrintInfo() const;  // sdmmc_card_print_info(stdout, card_)
```

Init 流程：
1. 拷贝 dev_config，覆盖 host_id = spi_bus.GetHostId()
2. `sdmmc_host_t host = SDSPI_HOST_DEFAULT();`
3. `esp_vfs_fat_sdspi_mount(base_path, &host, &cfg, &mount_config, &card_)`

Deinit 流程：`esp_vfs_fat_sdcard_unmount(base_path_.c_str(), card_)`

---

## sd_mmc

### SdMmcMountConfig : public esp_vfs_fat_mount_config_t
同 SdSpiMountConfig 构造

### SdMmcSlotConfig : public sdmmc_slot_config_t
构造参数：`clk, cmd, d0, d1..d7=NC, width=SDMMC_SLOT_WIDTH_DEFAULT, cd=SDMMC_SLOT_NO_CD, wp=SDMMC_SLOT_NO_WP, flags=0`

### class SdMmc
```cpp
bool Init(int slot, const SdMmcSlotConfig&,
          const SdMmcMountConfig&, const char* base_path="/sdcard");
bool Deinit();
bool IsMounted() const;
const char* GetBasePath() const;
sdmmc_card_t* GetCard() const;
void PrintInfo() const;
```

Init 流程：
1. `sdmmc_host_t host = SDMMC_HOST_DEFAULT(); host.slot = slot;`
2. `esp_vfs_fat_sdmmc_mount(base_path, &host, &slot_config, &mount_config, &card_)`

> 使用 legacy API (`driver/sdmmc_host.h`)，不使用新 API `sd_host_create_sdmmc_controller()`

---

## fatfs

### FatFsMountConfig : public esp_vfs_fat_mount_config_t
构造参数：`format_if_mount_failed=false, max_files=5, allocation_unit_size=0`

### class FatFs
成员：`Logger& logger_`, `wl_handle_t wl_handle_=WL_INVALID_HANDLE`, `bool mounted_`, `std::string base_path_`, `std::string partition_label_`

```cpp
bool Init(const char* partition_label, const FatFsMountConfig&,
          const char* base_path="/ffat");
bool Deinit();
bool GetInfo(uint64_t& total_bytes, uint64_t& free_bytes);
bool Format();
wl_handle_t GetWlHandle() const;
bool IsMounted() const;
const char* GetBasePath() const;
```

---

## spiffs

### SpiffsConfig : public esp_vfs_spiffs_conf_t
构造参数：`base_path, partition_label, max_files=5, format_if_mount_failed=false`

### class Spiffs
成员：`Logger& logger_`, `bool mounted_`, `std::string partition_label_`

```cpp
bool Init(const SpiffsConfig&);
bool Deinit();
bool GetInfo(size_t& total_bytes, size_t& used_bytes);
bool Format();
bool Check();
bool IsMounted() const;
const char* GetPartitionLabel() const;
```

label 为空字符串时传 nullptr（使用默认 SPIFFS 分区）

---

## Phase 2：CMakeLists.txt 追加依赖

```cmake
set(REQUIRES_ESP_IDF
  ...
  "fatfs"
  "wear_levelling"
  "spiffs"
  "esp_driver_sdmmc"
  "esp_driver_sdspi"
  "sdmmc"
)
```

---

## IDF 6.0.1 头文件对应关系

| 头文件 | IDF Component |
|---|---|
| `esp_vfs_fat.h` | `fatfs` |
| `driver/sdspi_host.h` | `esp_driver_sdspi` |
| `driver/sdmmc_host.h` (legacy) | `esp_driver_sdmmc` |
| `sdmmc_cmd.h` | `sdmmc` |
| `wear_levelling.h` | `wear_levelling` |
| `esp_spiffs.h` | `spiffs` |

---

## Decisions
- GLOB 自动收集 `src/wrapper/*.cpp`，新 .cpp 无需手动加入 SRCS
- `esp_vfs_fat_sdspi_mount` 内部调用 sdspi_host_init+init_device，SdSpi 不额外调用
- `esp_vfs_fat_sdcard_unmount` 内部调用 sdspi_host_remove_device，Deinit 不额外调用
