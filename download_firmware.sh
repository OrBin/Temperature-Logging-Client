#!/bin/bash
set -e

micropython_base_url="https://micropython.org"
latest_file_path=$(
  curl -sXGET "${micropython_base_url}/download/all/" |
  grep -P "esp8266-\d{8}-v(\d+\.)*\d+\.bin" |
  cut -d'"' -f2 |
  sort |
  tail -1
)
latest_file_name=$(echo "${latest_file_path}" | rev | cut -d"/" -f1 | rev)
latest_file_url="${micropython_base_url}/${latest_file_path}"
wget -q "${latest_file_url}" -O "./${latest_file_name}"
echo "./${latest_file_name}"
