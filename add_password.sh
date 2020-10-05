#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $DIR

gnome-terminal --window --title "Add new password" -- python3 ./controller/add_password.py config.json
