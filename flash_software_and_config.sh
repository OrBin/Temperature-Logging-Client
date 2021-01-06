#!/bin/bash
set -e

port=$1
ampy --port $port put config.py
ampy --port $port put main.py
