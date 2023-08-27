# TinyUSB Reset Bug

Example project demonstrating a workaround for the bug (espressif/esp-idf#9826)[https://github.com/espressif/esp-idf/issues/9826] where the USB/JTAG interface stops working after TinyUSB is used, even after a reboot.

It depends on this patch to esp_tinyusb: https://github.com/espressif/idf-extra-components/pull/229 (which should automatically be included because the idf_component.yml file points to that PR's branch).

In short, add this code to switch back on the USB/JTAG interface after using TinyUSB:

```c++
#include <esp_private/usb_phy.h>
```
```c++
tinyusb_driver_uninstall();
usb_phy_config_t phy_conf = {
    .controller = USB_PHY_CTRL_SERIAL_JTAG,
};
usb_phy_handle_t jtag_phy;
usb_new_phy(&phy_conf, &jtag_phy);
```

## Steps to reproduce

Build and flash the project.

Optionally watch the logs initially using the USB/JTAG interface (possibly replacing `/dev/tty.usbmodem*` with the right port):

```
idf.py monitor --no-reset --port /dev/tty.usbmodem*
```

The program does this:
* Waits 10s
* Initializes TinyUSB (USB/JTAG goes away as expected)
* Waits 10 more seconds
* Deinitializes TinyUSB and re-initialises the USB/JTAG interface (USB/JTAG comes back)
* Waits 10 more seconds
* Calls `esp_restart` (to demonstrate USB/JTAG interface working after reboot)

## Previous workarounds attempted:

I tried several other things in my quest to fix this issue:

* Deep sleep for 5s (thinking possibly that would actually power cycle the USB peripheral). This did not fix the issue.
* `periph_module_disable(usb_otg_periph_signal.module);`. This did not fix the issue.
* Use `usb_del_phy` to delete the TinyUSB phy then reboot. I added this to tinyusb.c and called it from usb-reset-bug.cpp before restarting. This did not fix the issue. This was very close to the final solution, the missing part was to re-initialize the USB/JTAG interface after calling `usb_del_phy`.
    ```
    esp_err_t tinyusb_driver_uninstall()
    {
        return usb_del_phy(phy_hdl);
    }
    ```
