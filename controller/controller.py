#!/usr/bin/env python3

import os
import sys
import time
import shutil
import socket
import getpass
import clipboard
import threading
from pynput import keyboard


class Controller:
    def __init__(self, **kwargs):
        self.__args = kwargs
        self.user = str()
        self.sock = None
        self.__connected = False

        self.__receive_daemon = None

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
            return self.get_paths()['assets']
        except KeyError:
            return ''

    def get_bin_dir(self) -> str:
        try:
            return self.get_paths()['bin']
        except KeyError:
            return ''

    def get_socket_address(self) -> str:
        try:
            return self.get_paths()['socket']
        except KeyError:
            return ''

    def get_log_file(self) -> str:
        try:
            return self.get_paths()['log_file']
        except KeyError:
            return ''

    def get_user(self) -> str:
        return self.user

    def is_daemon(self) -> bool:
        try:
            return self.__args['daemonize_controller']
        except KeyError:
            return False

    def connect(self) -> bool:
        if self.connected():
            return True

        try:
            self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM, 0)
            self.sock.connect(self.get_socket_address().encode('utf-8'))
        except Exception as ex:
            self.__log('[-] connect error:', ex)
            self.trigger_exit(1)
            return False
        
        self.__connected = True
        return True

    def connected(self) -> bool:
        return self.__connected

    def set_user(self, user: str) -> None:
        self.user = user

    def set_password(self, password: str) -> bool:
        try:
            self.sock.send(password.encode('utf-8'))
        except Exception as ex:
            self.__log('[-] send error:', ex)
            self.trigger_exit(1)
            return False
        return True

    def login(self):
        try:
            self.set_user(self.__args['user'])
        except KeyError:
            self.set_user(getpass.getuser())

        try:
            return self.set_password(self.__args['password'])
        except KeyError:
            return self.set_password(getpass.getpass())

    def start_cryptex_daemon(self):
        try:
            os.remove(self.get_socket_address())
        except FileNotFoundError:
            pass
        
        start_service_command = '{bin}/cryptex -daemon -address {address} -log {log}'.format(
            bin=self.get_bin_dir(), 
            address=self.get_socket_address(), 
            log=self.get_log_file()
        )
        os.system(start_service_command)

    def start(self):
        self.start_cryptex_daemon()
        time.sleep(.25)
        self.__log('[+] Started cryptex daemon.')

        if self.connect() and self.login():
            self.__receive_daemon = threading.Thread(target=self.__receive_data, 
            name="Receive Data", 
            daemon=True)
            self.__receive_daemon.start()

            threading.Thread(target=self.__monitor_keyboard, 
            name="Keyboard Monitor", 
            daemon=True).start()

            self.__log('[+] Started keyboard monitor and data receiving daemons.')
            self.wait()

    def wait(self):
        self.__receive_daemon.join()

    def stop(self):
        try:
            self.sock.send(b'exit')
            self.__log('[+] Sent \"exit\" command.')
        except Exception as ex:
            self.__log('[-] send error:', ex)
        finally:
            self.trigger_exit(1)

    def trigger_exit(self, exit_code: int = 0):
        try:
            if self.__args['exit_on_error']:
                self.__log('[+] Exit.')
                exit(exit_code)
        except KeyError:
            pass

    def get(self):
        details = {
            'assets': self.get_assets_dir(),
            'user': self.get_user(),
            'filename': clipboard.paste()
        }

        asset_path = '{assets}/{user}/{filename}'.format(**details)
        
        command = '-decrypt -in ' + asset_path

        try:
            self.sock.send(command.encode('utf-8'))
        except Exception as ex:
            self.__log('[-] send error:', ex)
            self.trigger_exit(1)

    def receive_data(self) -> str:
        try:
            return self.sock.recv(1024).decode('utf-8')
        except Exception as ex:
            self.__log('[-] ERROR: recv error:', ex)
            self.trigger_exit(1)

    def __log(self, data: str) -> None:
        if self.is_daemon():
            try:
                with open(self.get_log_file(), 'a') as log_file:
                    log_file.write('CONTROLLER: ' + data + '\n')
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
            self.__log('[-] Unknown Exception Occured')

    def __receive_data(self):
        fails = 0
        keyboard_controller = keyboard.Controller()

        while True:
            received = self.receive_data()

            if not received:
                fails += 1
            else:
                keyboard_controller.type(received)
                fails = 0

            del received

            if fails == 15:
                return
