#!/usr/bin/python3

import os
import json


CWD = os.getcwd()
EXEC = CWD + "/launcher.sh"
ICON = CWD + "/icons/icon.png"

template = "[Desktop Entry]\n"\
"Version=1.0\n"\
"Name[en_US]=Cryptex\n"\
"GenericName[en_US]=Cryptex\n"\
"Comment[en_US]=Passwords Manager\n"\
"Exec={exec}\n"\
"Path={wd}\n"\
"Icon={icon}\n"\
"Terminal=false\n"\
"Type=Application\n"\
"Categories=Application"

desktop_entry = template.format(exec=EXEC, wd=CWD, icon=ICON)

with open('./cryptex.desktop', 'w') as file:
    file.write(desktop_entry)
    file.close()

os.system('cp -v ./cryptex.desktop ~/.local/share/applications/cryptex.desktop')


with open('./config.json', 'r') as cfgfile:
    configuration = json.load(cfgfile)
    cfgfile.close()

    data = configuration['paths']['data']
    assets = configuration['paths']['assets']
    log_file = configuration['paths']['log_file']
    tmp = configuration['paths']['tmp']

    os.system('mkdir -p -v ' + data)
    os.system('mkdir -p -v ' + assets)
    os.system('mkdir -p -v ' + tmp)

    os.system('touch ' + log_file)

    os.system('chmod -v +x ./*.sh')
