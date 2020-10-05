import os
import shutil

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
"Terminal=true\n"\
"Type=Application\n"\
"Categories=Application"

desktop_entry = template.format(exec=EXEC, wd=CWD, icon=ICON)

with open('cryptex.desktop', 'w') as file:
    file.write(desktop_entry)
    file.close()

shutil.copyfile('./cryptex.desktop', os.path.expanduser('~') + '/.local/share/applications/cryptex.desktop')
print('[+] Desktop entry created.')

os.system('chmod +x ./*.sh')
