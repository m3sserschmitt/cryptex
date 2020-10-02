#!/usr/bin/env python3

from argparse import ArgumentParser
from os.path import abspath
from get_libs import get_libcryptography

import subprocess
import getpass
import json
import os

# config.json file template

configurations = {
    "hotkeys": {
        "get": "<alt>+c",
        "stop": "<alt>+x"
    },

    "paths": {
        "root": abspath(''),
        "dist": "",
        "bin": "",
        "assets": "",
        "user": "",
        "socket": "",
        "controller_pid": "",
        "log_file": ""
    },

    "exit_on_error": True,
    "daemonize_controller": True
}

##################################
## Parse command line arguments ##
##################################

args_parser = ArgumentParser()
args_parser.add_argument('-defaults', action='store_true', dest='defaults', default=False)
args_parser.add_argument('-out', dest='out', required=False)
cmd_args = vars(args_parser.parse_args())


##################################
## Get configurations from user ##
##################################

PATHS = configurations['paths']

if not cmd_args['defaults']:
    print('[+] You have to setup some variables. Press Enter if you want to keep default values.')

    root_directory = input(
        '[+] Enter root directory (' + PATHS['root'] + ', by default): ')

    if root_directory:
        PATHS['root'] = root_directory.rstrip('/')

PATHS['dist'] = PATHS['root'] + '/dist'
PATHS['assets'] = PATHS['dist'] + '/assets'
PATHS['user'] = PATHS['assets'] + '/' + getpass.getuser()
PATHS['bin'] = PATHS['dist'] + '/bin'
PATHS['socket'] = PATHS['dist'] + '/daemon.socket'
PATHS['log_file'] = PATHS['dist'] + '/daemon.log'
PATHS['controller_pid'] = PATHS['root'] + '/controller.pid'

LIBCRYPTOGRAPHY_DIR = os.path.dirname(os.path.realpath(__name__))

if not cmd_args['defaults']:
    log_file = input('[+] Enter log file path (' + PATHS['log_file'] + ', by default): ')

    if log_file:
        PATHS['log_file'] = log_file.rstrip('/')

    print('\n[+] You set this values:')
print('paths =', json.dumps(configurations['paths'], indent=2), '\n')

##########################
## Create required files ##
##########################


for directory in [PATHS['dist'], PATHS['assets'], PATHS['bin'], PATHS['user']]:
    try:
        os.mkdir(directory)
        print('[+] Created directory ', directory, '.', sep='')
    except FileExistsError:
        print('[-] Error creating directory',
              directory, ': directory already exists.')

try:
    with open(cmd_args['out'], 'w') as config_file:
        json.dump(configurations, config_file, indent=2)
        config_file.close()

        print('[+] File', cmd_args['out'], 'created.')
except Exception:
    print('[-] Warning: Configuration file not created.')


############################
## get required libraries ##
############################

print('[+] Downloading libcryptography source code...')
try:
    get_libcryptography(LIBCRYPTOGRAPHY_DIR)
except Exception:
    print('[-] Something went wrong while cloning libcryptography repository.')
    
print('\n[+] Library compilation should start automatically. Otherwise type \"make deps\"\n')
