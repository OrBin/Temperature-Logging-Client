#!/bin/bash
set -e

echo "Downloading firmware file"
firmare_file_path=$(./download_firmware.sh)
echo "Downloaded ${firmare_file_path}"

port=`ls /dev/ttyUSB* 2>/dev/null | head -1`
echo "Flasing ${firmare_file_path} to port ${port}"
./flash_firmware.sh "$port" "$firmare_file_path"

sleep 3
echo "Flasing software and config to port ${port}"
./flash_software_and_config.sh "$port"
