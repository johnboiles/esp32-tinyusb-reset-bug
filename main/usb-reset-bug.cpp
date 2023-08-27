#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <tinyusb.h>
#include <driver/gpio.h>
#include <esp_private/usb_phy.h>

// For the `periph_module_disable` workaround attempt
// #include <soc/usb_otg_periph.h>
// #include <esp_private/periph_ctrl.h>
// extern const usb_phy_signal_conn_t usb_otg_periph_signal;

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

    // This does not work
    // periph_module_disable(usb_otg_periph_signal.module);

    // THIS WORKS!!
    // It requires this patch to tinyusb.c:
    // https://github.com/espressif/idf-extra-components/pull/229
    tinyusb_driver_uninstall();
    usb_phy_config_t phy_conf = {
        .controller = USB_PHY_CTRL_SERIAL_JTAG,
    };
    usb_phy_handle_t jtag_phy;
    usb_new_phy(&phy_conf, &jtag_phy);

    // Blink LED to indicate USB has been reset
    gpio_num_t ledPin = GPIO_NUM_7;
    gpio_set_direction(ledPin, GPIO_MODE_OUTPUT);
    gpio_set_level(ledPin, 0);

    // Wait a bit so idf.py monitor --no-reset can see the logs
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP_LOGI(tag, "USB/JTAG is back!");

    vTaskDelay(10000 / portTICK_PERIOD_MS);

    // This does not work
    // esp_sleep_enable_timer_wakeup(5000000);
    // esp_deep_sleep_start();

    ESP_LOGI(tag, "Rebooting");
    esp_restart();
}
