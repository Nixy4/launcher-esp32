#include "sdkconfig.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>

#include "board/m5stack/tab5.hpp"
#include "wrapper/freertos.hpp"
#include "wrapper/logger.hpp"

wrapper::Task board_init(
    "BoardInit",
    [](void*)
    {
        wrapper::M5StackTab5& board = wrapper::M5StackTab5::GetInstance();
        board.InitCoreBusAndIoExpander();
        board.InitDisplay();
        board.GetLvglPort().SetRotation(LV_DISPLAY_ROTATION_90);
        board.InitAudio();

        board.TestKeyboard();
    },
    nullptr,
    8192,
    5);
//!----------------------------------------------------------------------------------------

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
