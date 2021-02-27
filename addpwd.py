#!/usr/bin/python3

import os
import json
import getpass
import subprocess

if os.getuid():
    print('[-] Please run the script as root.')
    exit(0)

password = getpass.getpass('Enter New Password: ')

while password != getpass.getpass('Confirm Password: '):
    print('Passwords don\'t match!')

identifier = input('Enter Password Identifier: ')

with open('./config.json', 'r') as cfgfile:
    configuration = json.load(cfgfile)
    cfgfile.close()

    assets = configuration['paths']['assets'].rstrip('/')
    out_file = assets + '/' + identifier
    cryptutil = configuration['paths']['cryptutil']
    
    if os.path.exists(out_file):
        answer = ''
        while answer not in ['y', 'n']:
            answer = input(
                'File already exists! Would you like to override it? (y/n): ')

        if answer == 'n':
            exit(0)

        os.system('chattr -iu ' + out_file)
    
    master_password = getpass.getpass('Enter Master Password: ')
    print()

    command = [
        cryptutil,
        '-encrypt',
        '-out', out_file
        ]

    os.system('chattr -V -iu ' + assets)

    process = subprocess.Popen(command,
                               stdin=subprocess.PIPE,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT
                               )

    output = process.communicate(
        input=(master_password + '\000' + password).encode())[0].decode('utf-8')
        
    os.system('chattr -V +iu ' + out_file)
    os.system('chattr -V +iu ' + assets)

    print(output, end='')
