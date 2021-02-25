#!/bin/bash

./env.sh
gnome-terminal --window --title "Enter Master Password" -- python3 ./controller/main.py ./config.json
