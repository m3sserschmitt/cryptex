#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $DIR

gnome-terminal --window --title "Enter Master Password" -- python3 -m controller config.json