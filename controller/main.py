#!/usr/bin/python3

import os
import sys
import json
import getpass
from daemonize import Daemonize
import controller as cryptex_controller

config = dict()

try:
    config = sys.argv[1]
except IndexError:
    print('[-] ERROR: You have to pass configuration file as command line argument.')
    exit(1)

try:
    with open(config, 'r') as config_file:
        config = json.load(config_file)
        config_file.close()
except FileNotFoundError:
    print('[-] ERROR: Configuration file not found.')
    exit(1)

config['password'] = getpass.getpass()
controller = cryptex_controller.Controller(**config)

if config['daemon']:
    Daemonize(app="cryptex-controller",
              pid=config['paths']['pid'],
              action=controller.start, chdir=os.path.abspath('./')).start()
else:
    controller.start()
