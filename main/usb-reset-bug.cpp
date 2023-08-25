#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <tinyusb.h>
#include <driver/gpio.h>

// For the `periph_module_disable` workaround attempt
// #include <soc/usb_otg_periph.h>
// #include <esp_private/periph_ctrl.h>
// extern const usb_phy_signal_conn_t usb_otg_periph_signal;

// For the `usb_del_phy` workaround attempt
// #include <esp_private/usb_phy.h>
// Must add this to tinyusb.c since you can't extern static variables
// usb_phy_handle_t *phy_hdl_ptr = &phy_hdl;
// extern "C" usb_phy_handle_t *phy_hdl_ptr;

// For the deep sleep workaround attempt
#include <esp_sleep.h>

static const char *tag = "USB";

extern "C" void app_main(void) { // NOLINT(readability-identifier-naming)
    // Wait a beat so we can see the logs when using `idf.py monitor --no-reset`
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    ESP_LOGI(tag, "Waiting 10s before initializing USB");

    vTaskDelay(10000 / portTICK_PERIOD_MS);

    ESP_LOGI(tag, "TinyUSB initialization. Rebooting in 10s");
    const tinyusb_config_t tusbCfg = {
        .device_descriptor = nullptr,
        .string_descriptor = nullptr,
        .string_descriptor_count = 0,
        .external_phy = false,
        .configuration_descriptor = nullptr,
        .self_powered = false,
        .vbus_monitor_io = 0,
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusbCfg));

    vTaskDelay(10000 / portTICK_PERIOD_MS);

    // TODO: The goal is to understand why the USB/JTAG device does not come back after esp_restart

    // This does not work
    // usb_del_phy(*phy_hdl_ptr);

    // This does not work
    // periph_module_disable(usb_otg_periph_signal.module);
    
    // Blink LED to indicate we're rebooting
    gpio_num_t ledPin = GPIO_NUM_7;
    gpio_set_direction(ledPin, GPIO_MODE_OUTPUT);
    gpio_set_level(ledPin, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // This does not work
    // esp_sleep_enable_timer_wakeup(5000000);
    // esp_deep_sleep_start();

    esp_restart();
}
