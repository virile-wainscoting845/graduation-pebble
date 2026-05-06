# Hardware guide

## Reference device — M5StickS3 K150

The Phase 0 reference build targets the **M5StickS3 (SKU K150)**, a small ESP32-S3-based handheld from M5Stack.

| Component | Spec |
|---|---|
| MCU | ESP32-S3-PICO-1-N8R8 (dual Xtensa LX7 @ 240 MHz, AI accelerator) |
| Memory | 8 MB Flash + 8 MB OPI PSRAM |
| Display | 1.14" ST7789P3 IPS LCD, 135 × 240 px, 16-bit color |
| IMU | Bosch BMI270 (6-axis) on I²C address 0x68 |
| Audio | Everest ES8311 mono codec + AW8737 amp + MEMS mic |
| PMIC | M5Stack M5PM1 power management |
| Connectivity | Wi-Fi 4, BLE 5, IR TX/RX |
| Battery | 250 mAh LiPo |
| USB | Native USB-Serial-JTAG (CDC + JTAG without external bridge) |

Buy from: [M5Stack official store](https://shop.m5stack.com/products/m5sticks3-esp32s3-mini-iot-dev-kit), [OpenELAB](https://openelab.com/products/m5sticks3-esp32s3-mini-iot-dev-kit), [Pi Hut](https://thepihut.com/products/m5sticks3-esp32s3-mini-iot-dev-kit), [Botland](https://botland.store/stick-series/27815-m5sticks3-iot-development-module-esp32-s3-pico-1-m5stack-k150.html), Mouser, DigiKey.

## Button physical layout

This is the painful part — the M5StickS3 docs are vague about which physical button is which logical button. Confirmed by hardware testing 2026-05-06:

| Physical button | Logical | GPIO | Reads from ESP32? |
|---|---|---|---|
| **Front, large round button (M5 logo)** | BtnA | GPIO 11 | ✅ via `M5.BtnA.isPressed()` |
| **Right side, small button** | BtnB | GPIO 12 | ✅ via `M5.BtnB.isPressed()` |
| **Left side, small black button** | PWR | (M5PM1 PMIC) | ❌ ESP32 cannot read this directly |

Implication: any in-firmware button feature must use BtnA and/or BtnB. The PWR button is reserved for PMIC-level functions (boot, reset, download mode entry).

## Flashing workflow

### First-time flash (factory firmware blocks USB-CDC)

1. Connect via USB-C.
2. **Long-press the LEFT side small (PWR) button for ~2 seconds**, releasing as soon as the **internal green LED starts blinking**.
3. The device enumerates as USB JTAG/serial debug unit (VID `0x303A`, PID `0x1001`) on a new COM port (typically COM9 on Windows; check your device manager).
4. Update `upload_port` in `platformio.ini` to that COM number.
5. Run `pio run -t upload -e m5sticks3`.
6. After the flash, the device hard-resets back to the user firmware (PID `0x8120`).

### Subsequent flashes

If the firmware was flashed by graduation-pebble itself, USB-CDC auto-reset usually works without manual download mode. If not, repeat the manual download mode dance.

### Common error: `Could not open COM9`

The device's USB identity changes between download mode (`PID 0x1001`) and running firmware (`PID 0x8120`), and Windows assigns different COM numbers to each. Always re-check the COM number after a power-cycle.

```powershell
Get-PnpDevice -PresentOnly | Where-Object { $_.InstanceId -match 'VID_303A&PID_1001' }
```

## Power-off

There are TWO ways to turn the device off:

1. **Soft (firmware-triggered)**: Hold BtnA + BtnB simultaneously for 2 seconds. The firmware calls `M5.Power.powerOff()` which signals the M5PM1 PMIC to fully disconnect the battery.
2. **Hardware (PMIC-only)**: Long-press the PWR button >6 seconds *while USB is unplugged*. With USB plugged in, the PMIC keeps the device powered no matter how long you hold PWR — USB 5 V latches the PMIC on.

## Adapting to other boards

If you don't have an M5StickS3, the firmware should run with light edits on:

- **M5StickC PLUS / PLUS2**: Same form factor, AXP192 PMIC instead of M5PM1. M5Unified abstracts most of this. Display is also 135×240 ST7789, so layout fits.
- **M5AtomS3**: Square 128×128 display — major layout rework needed.
- **Generic ESP32-S3 + ST7789**: Drop M5Unified, use TFT_eSPI directly. Buttons via raw GPIO.

PRs implementing other ports are welcome.
