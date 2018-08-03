# Temperature Logging Client
The logging device is a [NodeMCU ESP8266](http://nodemcu.com/index_en.html), running [MicroPython firmware](https://micropython.org/) (was previously running [NodeMCU firmware](https://github.com/nodemcu/nodemcu-firmware)).
There is also a web server to support saving, getting and watching the data. Code can be found [here](https://github.com/OrBin/Temperature_Logging-Server).

This page contains instructions to install/update the code files for the logging device even though it is (most of the time) in deep sleep mode.

## Requirements 

## Clean Installation
For a clean install, please take the following steps:
1. Disconnect any interrupt pin connections (for example, D0 to RST).
2. Disconnect the NodeMCU's cable from the computer.
3. Click and hold the FLASH button on the NodeMCU.
4. **(Don't leave the FLASH button)** Connect the NodeMCU's cable to the computer.
5. **(Don't leave the FLASH button)** Clear the device's flash:  `esptool.py --port <Your-Port> --baud 460800 erase_flash`. When the command begins its operation, you can leave the FLASH button.
6. Flash MicroPython's firmware: `esptool.py --port <Your-Port> --baud 460800 write_flash --flash_size=detect -fm dio 0 esp8266-micropython-firmware.bin`
7. Put the required code files in the device using `ampy`:
    * `ampy --port <Your-Port> put boot.py`
    * `ampy --port <Your-Port> put config.py`
    * `ampy --port <Your-Port> put main.py`

## Updating code files
This method should be used if you only want to update your .py files.
1. Disconnect any interrupt pin connections (for example, D0 to RST).
2. Disconnect the NodeMCU's cable from the computer.
3. Click and hold the FLASH button on the NodeMCU.
4. **(Don't leave the FLASH button)** Connect the NodeMCU's cable to the computer.
5. **(Don't leave the FLASH button)** Clear the file system region in the device's flash:  `esptool.py --port <Your-Port> --baud 460800  erase_region 626688 28672`. When the command begins its operation, you can leave the FLASH button.
6. Put the required code files in the device using `ampy`:
    * `ampy --port <Your-Port> put boot.py`
    * `ampy --port <Your-Port> put config.py`
    * `ampy --port <Your-Port> put main.py`
