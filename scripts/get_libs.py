import sys
import os


def get_libcryptography(path: str) -> None:
    clone = "git clone https://github.com/m3sserschmitt/cryptography.git"

    os.chdir(path)
    os.system(clone)