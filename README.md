# twatchbuilder-arduino

Source code for the TWatchBuilder firmware (https://twatchbuilder.com) for the T-WATCH-2020 watch.

## Prerequisites
To compile the code you need:
- Last Arduino IDE version. You can download from https://www.arduino.cc/en/software
- The **esp32** board. You can follow this tutorial: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
- The **TTGO T-Watch library** that you can download from here: https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library

## Installation
- Download the code from repository
- Rename the folder to "MyWatch"
- Open the "MyWatch.ino" file in the Arduino IDE
- Edit the "defines.h" file and set the T-Watch-2020 version do you want (LILYGO_WATCH_2020_V1, LILYGO_WATCH_2020_V2, LILYGO_WATCH_2020_V3)
- Select the "TTGO T-Watch" board
- Select the board revision. It depends on the watch's version.
- Build the project
- Install on your T-WATCH

## Collaboration
If you want to share new improvements or fix an bug, please make a Pull Request. Our team wil review it. **However, only changes that are in line with our vision will be included**.
