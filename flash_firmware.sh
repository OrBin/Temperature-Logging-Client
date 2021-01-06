#!/bin/bash
set -e

port=$1
firmare_file_path=$2

esptool.py --port $port --baud 460800 erase_flash
esptool.py --port $port --baud 460800 write_flash --flash_size=detect -fm dio 0 $firmare_file_path
