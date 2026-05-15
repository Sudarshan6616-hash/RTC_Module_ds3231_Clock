# RTC Interface Using DS3231 Module

## Overview
This project interfaces an Arduino with a DS3231 real-time clock module and a 16x2 LCD to display current time and date continuously. Four push buttons are used to switch modes and adjust time/date values directly from the hardware setup.

## Features
- Real-time display of time and date on a 16x2 LCD.
- Manual setting of time and date using four push buttons.
- I2C-based communication with the DS3231 RTC and LCD.
- Leap-year and month-length validation during date editing.
- Anti-flicker LCD update logic for stable output.

## Hardware Used
- Arduino board
- DS3231 RTC module
- 16x2 LCD with I2C interface
- Four push buttons
- Breadboard and jumper wires

## How It Works
On startup, the Arduino initializes Serial, I2C, the LCD, and the RTC module. If the RTC is not detected, the display shows `RTC FAIL` and the program stops. In normal mode, the LCD shows the current time on the first line and the date on the second line.

Pressing the mode button cycles through `MODE_DISPLAY`, `MODE_SET_TIME`, and `MODE_SET_DATE`. In the setting modes, the other buttons increment hour/minute/second or day/month/year, and the updated values are written back to the DS3231 using `rtc.adjust()`.

## Software Logic
The code uses `Wire.h` for I2C communication, `RTClib.h` for DS3231 access, and `LiquidCrystal_I2C.h` for the display. The LCD is initialized at address `0x27`, and the RTC object is created as `RTC_DS3231 rtc`.

To prevent invalid date entry, the code includes `daysInMonth()` and `isLeapYear()` helper functions. The display refresh is controlled by `lastNow`, `lastMode`, and `needsUpdate` so the LCD only updates when needed.

## Button Mapping
- Button 1: Hour or day increment
- Button 2: Minute or month increment
- Button 3: Second or year increment
- Button 4: Mode selection

## Working Modes
| Mode | LCD Output | Function |
|---|---|---|
| Display | TIME / DATE | Shows live clock and calendar values |
| Set Time | SET / HR, MN, SC | Allows editing of time fields |
| Set Date | SET / DD, MM, Y | Allows editing of date fields |

## Result
The system successfully reads time and date from the DS3231, displays them on the LCD, and supports manual editing through push-button controls. The prototype demonstrates reliable RTC interfacing, real-time display, and user input handling in a compact embedded system.

## Repository Contents
- `RTC_Module_Clock.ino` — Arduino source code
- Project images — hardware and LCD demonstration photos
