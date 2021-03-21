#!/usr/bin/python3

import os
import datetime
import subprocess
from pynput import keyboard
import pyperclip


class Controller:
    def __init__(self, **kwargs):
        self.__args = kwargs

    def get_hotkeys(self) -> dict:
        try:
            return self.__args['hotkeys']
        except KeyError:
            return {}

    def get_paths(self) -> dict:
        try:
            return self.__args['paths']
        except KeyError:
            return {}

    def get_assets_dir(self) -> str:
        try:
            return os.path.abspath(self.get_paths()['assets']).rstrip('/') + '/'
        except KeyError:
            return ''

    def get_log_file(self) -> str:
        try:
            return os.path.abspath(self.get_paths()['log_file'])
        except KeyError:
            return ''

    def get_cryptutil(self) -> str:
        try:
            return self.get_paths()['cryptutil']
        except KeyError:
            return ''

    def is_daemon(self) -> bool:
        try:
            return self.__args['daemon']
        except KeyError:
            return False

    def start(self):
        self.__log('[+] Started cryptex.')
        self.__monitor_keyboard()

    def stop(self):
        self.__log('[+] Sent \"exit\" command.')
        exit()

    def get(self):
        # read clipboard
        clipboard = pyperclip.paste()

        # if clipboard empty, return
        if not clipboard:
            return

        # get asset path
        asset = self.get_assets_dir() + clipboard

        try:
            # try to open file
            with open(asset, 'rb') as filep:
                # if successful, clear clipboard
                pyperclip.copy('')

                # run command
                command = [self.get_cryptutil(), '-decrypt', '-silent',
                           '-log', self.get_log_file()]

                process = subprocess.Popen(
                    command,
                    stdin=subprocess.PIPE,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT
                )

                # get back result
                result = process.communicate(
                    input=self.__args['password'].encode(
                        'utf-8') + b'\000' + filep.read(1024)
                )[0]

                keyboard.Controller().type(result.decode('utf-8'))

        except Exception as ex:
            self.__log('[-] Exception occurred:' + str(ex))

    def __log(self, data: str) -> None:
        now = datetime.datetime.now()

        if self.is_daemon():
            try:
                with open(self.get_log_file(), 'a') as log_file:
                    log_file.write(now.strftime(
                        '%H:%M:%S on %A, %B the %dth, %Y') + ': ' + data + '\n')
                    log_file.close()
            except FileNotFoundError:
                pass
        else:
            print(data)

    def __monitor_keyboard(self):
        hk = self.get_hotkeys()

        try:
            with keyboard.GlobalHotKeys({
                hk['get']: self.get,
                hk['stop']: self.stop,
            }) as hotkeys:
                hotkeys.join()

        except KeyError:
            self.__log('[-] ERROR: some required configurations are missing.')
        except Exception:
            self.__log('[-] Unknown Exception Occurred')
