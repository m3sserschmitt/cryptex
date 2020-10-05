import getpass
import shutil
import time
import json
import sys
import os


def add_password():
    try:
        config_file = sys.argv[1]
        with open(config_file, 'r') as file_pointer:
            config = json.loads(file_pointer.read())
            file_pointer.close()

    except IndexError:
        print('ERROR: you must provide config file.')
        exit(1)
    except FileNotFoundError:
        print('ERROR: config file not found.')
        exit(1)
    except Exception:
        print('ERROR: wrong config file format.')
        exit(1)

    identifier = input('[+] Enter identifier: ')
    plaintext_password = getpass.getpass('[+] Enter password for ' + identifier + ': ')
    
    while plaintext_password != getpass.getpass('[+] Confirm password for ' + identifier + ': '):
        print('[-] ERROR: Passwords don\'t match')

    assets_dir = config['paths']['assets'] + '/' + getpass.getuser()
    asset = assets_dir + '/' + identifier
    temp_file = os.path.abspath('temp_passwd_' + identifier)

    with open(temp_file, 'w') as file_pointer:
        file_pointer.write(plaintext_password)
        file_pointer.close()

    binary = config['paths']['bin'] + '/cryptex'

    command = '{bin} -in {in_file} -encrypt -out {out_file}'.format(bin=binary, 
    in_file=temp_file, out_file=asset)

    os.system(command)
    os.system('rm -v ' + temp_file)

    print('[+] Asset saved to', asset)
    input('[+] Press Enter to exit.')


if __name__ == '__main__':
    add_password()
