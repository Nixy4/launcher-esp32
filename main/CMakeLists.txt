idf_component_register(
  SRC_DIRS "."
  INCLUDE_DIRS "."
  REQUIRES nvs_flash driver esp_driver_i2c esp_wifi esp_event
  "wrapper-esp32"
)
