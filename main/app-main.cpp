#include "sdkconfig.h"
#include <cstdio>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>

//! Board Driver
//! ----------------------------------------------------------------------------------------
#include "board/m5stack/tab5.hpp"
#include "wrapper/freertos.hpp"
#include "wrapper/logger.hpp"
#include "wrapper/sd_mmc.hpp"
#include "wrapper/spiffs.hpp"

//! SPIFFS
//! ----------------------------------------------------------------------------------------
static wrapper::Logger spiffs_log("SPIFFS-Test");
static wrapper::SpiffsConfig spiffs_cfg("/spiffs",  // base_path
                                        "assets",   // partition_label
                                        5,          // max_files
                                        true);      // format_if_mount_failed
static wrapper::Spiffs spiffs(spiffs_log);

//! SD Card (SDIO 4-bit, Slot 0)
//! GPIO: CLK=43, CMD=44, D0=39, D1=40, D2=41, D3=42
//! ----------------------------------------------------------------------------------------
static wrapper::Logger sd_log("SD-Test");
static wrapper::SdMmcSlotConfig sd_slot_cfg(GPIO_NUM_43,        // CLK
                                            GPIO_NUM_44,        // CMD
                                            GPIO_NUM_39,        // D0
                                            GPIO_NUM_40,        // D1
                                            GPIO_NUM_41,        // D2
                                            GPIO_NUM_42,        // D3
                                            GPIO_NUM_NC,        // D4
                                            GPIO_NUM_NC,        // D5
                                            GPIO_NUM_NC,        // D6
                                            GPIO_NUM_NC,        // D7
                                            4,                  // width (4-bit)
                                            SDMMC_SLOT_NO_CD,   // CD
                                            SDMMC_SLOT_NO_WP);  // WP
static wrapper::SdMmcMountConfig sd_mount_cfg(false,            // format_if_mount_failed
                                              5);               // max_files
static wrapper::SdMmc sd(sd_log);

wrapper::Task board_init(
    "BoardInit",
    [](void*)
    {
        wrapper::M5StackTab5& board = wrapper::M5StackTab5::GetInstance();
        board.InitCoreBusAndIoExpander();
        board.InitDisplay();
        board.GetLvglPort().SetRotation(LV_DISPLAY_ROTATION_90);
        board.InitAudio();

        spiffs.Init(spiffs_cfg);
        spiffs.Test();
        spiffs.Deinit();

        sd.Init(SDMMC_HOST_SLOT_0, sd_slot_cfg, sd_mount_cfg, "/sdcard");
        sd.Test();
        sd.Deinit();

        board.InitKeyboard();
        board.GetKeyboard().Test();
    },
    nullptr,
    16384,  // 增大栈空间以容纳文件系统操作
    5);

//! Application Main
//! ----------------------------------------------------------------------------------------

namespace launcher
{

enum class Status : int8_t
{
    kOk = 0,

    // 通用错误
    kErrUnknown = -1,
    kErrInvalidArg = -2,
    kErrNotSupported = -3,
    kErrTimeout = -4,

    // 状态错误
    kErrNotInitialized = -10,
    kErrAlreadyInitialized = -11,
    kErrInvalidState = -12,

    // 资源错误
    kErrNoMemory = -20,
    kErrNoResource = -21,
};

class Hal
{
   public:
    virtual void Init();

    virtual void InitCore();

    virtual void InitDisplay();
    virtual void SetDisplayRotation(int rotation);
    virtual void SetDisplayBrightness(int percent);
    virtual void SetDisplayBacklight(bool on);
    virtual void SetDisplayPower(bool on);

    virtual void InitInputDevice();
    virtual void PollInputDevice();
};

class Gui
{
   public:
    virtual void Init();

    // virtual void InitDriver();  // 输出设备和输入设备
};

}  // namespace launcher

//!----------------------------------------------------------------------------------------
extern "C" void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    board_init.Create();
}
