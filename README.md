# twatchbuilder-arduino

TWatchBuilder firmware source code for the T-WATCH-2020 watch. You can visit our project on https://twatchbuilder.com

## Prerequisites
To compile the code you need:
- Last Arduino IDE version. You can download from https://www.arduino.cc/en/software
- The **esp32** board. You can follow this tutorial to install it: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
- The **TTGO T-Watch library** that you can download from here: https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library

## Installation
- Clone or download the code from repository
- Open the "twatchbuilder-arduino.ino" file in the Arduino IDE
- Edit the "defines.h" file and set the T-Watch-2020 version do you want (LILYGO_WATCH_2020_V1, LILYGO_WATCH_2020_V2, LILYGO_WATCH_2020_V3)
- Select the "TTGO T-Watch" board (this board is included when you install the **esp32** board)
- Select the board revision. It depends on the watch's version.
- Build the project
- Install on your T-WATCH

## Collaboration
If you want to share new improvements or fix an bug, please make a Pull Request. Our team wil review it. **However, only changes that are in line with our vision will be included**.
