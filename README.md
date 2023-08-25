# TinyUSB Reset Bug

Example project demonstrating a bug (espressif/esp-idf#9826)[https://github.com/espressif/esp-idf/issues/9826] where the USB/JTAG interface stops working after TinyUSB is used, even after a reboot.

## Steps to reproduce

Build and flash the project.

Optionally watch the logs initially using the USB/JTAG interface (possibly replacing `/dev/tty.usbmodem*` with the right port):

```
idf.py monitor --no-reset --port /dev/tty.usbmodem*
```

The program will wait 10s, then initialize TinyUSB, then wait 10 more seconds, then `esp_restart`. After the restart, the USB/JTAG interface will no longer work.

## Workarounds attempted:

* Use `usb_del_phy`. I added this to tinyusb.c and called it from usb-reset-bug.cpp. This did not fix the issue.
    ```
    esp_err_t tinyusb_driver_uninstall()
    {
        return usb_del_phy(phy_hdl);
    }
    ```
* Deep sleep for 5s (thinking possibly that would actually power cycle the USB peripheral). This did not fix the issue.
* `periph_module_disable(usb_otg_periph_signal.module);`. This did not fix the issue.
