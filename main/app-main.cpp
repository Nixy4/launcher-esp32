#include "sdkconfig.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>

#include "board/m5stack/tab5.hpp"
#include "wrapper/freertos.hpp"
#include "wrapper/logger.hpp"

using namespace wrapper;

Logger lmain("app-main");

Task board_init(
    "BoardInit",
    [](void *) {
      M5StackTab5 &board = M5StackTab5::GetInstance();
      board.InitCoreBusAndIoExpander();
      board.InitDisplay();
      board.GetLvglPort().SetRotation(LV_DISPLAY_ROTATION_90);
      board.InitAudio();

      board.TestKeyboard();
    },
    nullptr, 8192, 5);

extern "C" void app_main(void) {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  lmain.Info("Starting board initialization task...");
  if (!board_init.Create()) {
    lmain.Error("Failed to create board initialization task");
    return;
  }
}
