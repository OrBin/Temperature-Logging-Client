# Temperature Logging Client
The logging device is a [NodeMCU ESP8266](http://nodemcu.com/index_en.html), running [MicroPython firmware](https://micropython.org/) (was previously running [NodeMCU firmware](https://github.com/nodemcu/nodemcu-firmware)).
There is also a web server to support saving, getting and watching the data. Code can be found [here](https://github.com/OrBin/Temperature_Logging-Server).


## Clean Installation
For a clean install, please take the following steps:
1. Clear the microcontroller's flash:  `esptool.py --port <Your-Port> --baud 460800 erase_flash`
2. Flash MicroPython firmware: `esptool.py --port <Your-Port> --baud 460800 write_flash --flash_size=detect -fm dio 0 esp8266-micropython-firmware.bin`
3. Copy the required code files into the device using `ampy`:
    * `ampy --port <Your-Port> put config.py`
    * `ampy --port <Your-Port> put main.py`


## Clearing File System Region in Flash
If necessary, clear the file system region in the device's flash:  `esptool.py --port <Your-Port> --baud 460800  erase_region 626688 28672`


## Updating Code Files
Copy the required code files into the device using `ampy`:
    * `ampy --port <Your-Port> put config.py`
    * `ampy --port <Your-Port> put main.py`
