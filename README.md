# cryptex

Password manager & Cryptography tool.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine.

*__Please, read the instructions carefully! Cryptography & Passwords Management are not easy stuff and you should have a basic understanding of what you are doing!__*

### Prerequisites

You need OpenSSL library to be installed on your machine. Type this command in your terminal:

`openssl version`

If you see something similar to this: `OpenSSL 1.1.1g  21 Apr 2020`, it means that you have OpenSSL installed, otherwise if an error message is shown, you have to install the library in order to compile this project. Checkout [OpenSSL](https://www.openssl.org/) site for details. Usually, this library is installed on most Linux distros.

Firt of all, we need an extra library called `libcryptography.so`. Checkout [cryptography](https://github.com/m3sserschmitt/cryptography.git) for details.


### Installing

You need a copy of source code. Download the repository using git:

`git clone https://github.com/m3sserschmitt/cryptex.git`

Then:

`cd /your/local/repository/path`


Type:

```
cd cryptutil/build
make
```

then run command as root:

```
make install
```

Last command will create `/usr/local/bin/cryptutil` file, which is the executable for
basic cryptographic operations.

Next command should be ran as regular user (not root). It creates a desktop icon and some files & directories:

```
cd ../../
python3 ./setup.py
```

In order to make all files read-only, you can run following commands as root (optional, but recommended):

```
chattr -VR +iu /path/to/local/repository
chattr -V -i /path/to/local/repository/data/daemon.log
```

Last two commands will change attributes for all files located in local repository as *imutable* and *undeleteble*, excepting `./data/daemon.log`, to allow program to write logs.

Done!

### How it works?

### Passwords manager.

After the building process is done, you have to accomplish a couple of additional steps in order to use this project as a passwords manager.

Let's say you want to set a password for domain `example.com`.

Run as root:

`python3 ./addpwd.py`

You will be prompted to enter a password, to confirm the password and to add a password identifier (`example.com`, in this example). After that you will be prompted to enter *Master Password* (__keep it safe__ and make sure to __remember__ it in the future!).

Next time you want to set a password for another domain, you have to proceed the same (make sure to enter the __same *Master Password*__).

*_Usage:_*

Now, let's consider the case you want to login on `example.com` site. Click on Cryptex icon and enter your master password. Usually, a login process involves a username and a password. Enter your username. After that, select the domain name (in you address bar, for example), press `Ctrl+C`, to copy this domain name into clipboard. Click the password field to focus, and press `Alt+C`. Your password should now appear.

Keep in mind that you can use your username as an identifier for your password instead of domain name (actually, you can use anything you want). If you do so, you only have to replace `example.com` in previous instructions with whatever identifier you'd like.

### Cryptography tool.

You have installed a cryptographic tool, named `cryptutil`. 

Type `cryptutil -help`, for details about available commands.


### The background stuff.

When you're using the passwords manager, a daemon process called `controller` starts, and it is responsible for processing your commands (e.g. getting your passwords).
This terminates when you press `Alt+X`.

These hotkeys can be changed in configuration file called `config.json`. Take a look, it's intuitive and easy to do so.

## Authors

* **Romulus-Emanuel Ruja** <<romulus-emanuel.ruja@tutanota.com>>

## License

This project is licensed under the MIT License. Feel free to copy, modify and distribute it - see the [LICENSE](LICENSE) file for details.

## Disclaimer

Keep your master password at safe! Do not use a easy-to-guess master password! Be sure to remember your master password, otherwise your data is lost! This project cames with no warranty, use it on your own risk!