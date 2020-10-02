# cryptex

Password manager & cryptography tool.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine.

*__Please, read the instructions carefully! Cryptography it's not easy stuff and you should have a basic understanding of what you are doing!__*

### Prerequisites

You need OpenSSL library to be installed on your machine. Type this command in your terminal:

`openssl version`

If you see something similar to this: `OpenSSL 1.1.1g  21 Apr 2020`, it means that you have OpenSSL installed, otherwise if an error message is shown, you have to install the library in order to compile this project. Checkout [OpenSSL](https://www.openssl.org/) site for details. Usually, this library it's installed on most linux distros.

### Installing

You need a copy of source code. Download the repository using git:

`git clone https://github.com/m3sserschmitt/cryptex.git`

Then:

`cd /your/local/repository/path`

Firt of all, we need an extra library called `libcryptography.so`. Checkout [cryptography](https://github.com/m3sserschmitt/cryptography.git) for details.

Type:

`make config`

and this command will download and compile required library. After that we are ready to build the project:

`make`

If everything is OK, you can type `make setup` in order to create a desktop icon.

Done!

### How it works?

### Passwords manager.

After the building process is done, you have to accomplish a couple of additional steps in order to use this project as a passwords manager.

Let's say you want to set a password for domain `example.com`.
Create a file called `example.com` into `./dist/assets/your_username` (path is relative to your local repository, so make sure you perform this steps in your local repository directory) that contains your password for this site.

Use the following command:

`./dist/bin/cryptex -in /path/to/example.com -encrypt -out /path/to/example.com`

(Enter full paths)

You will be prompted to enter a password (this will be your master password, so keep it safe and make sure to remember it in the future). You are ready!

Next time you want to set a password for another domain, you have to proceed the same (make sure to enter the same master password).

Now, let's consider the case you want to login on `example.com` site. Click on Cryptex icon and enter your master password. Usually, a login process involves a username and a password. Enter your username. After that, select the domain name (in you address bar, for example), press `Ctrl+C`, in order to copy this domain name into clipboard. Click in the password field to focus, and press `Alt+C`. Your password should now appear.

Keep in mind that you can use your username as an identifier for your password instead of domain name (Actually, you can use anything you want). If you do so, you only have to replace the domain name in previous instructions with whatever identifier you'd like.

### Cryptography tool.

There is a file called `cryptutil.sh` in your local repository.

Let's make the things a little bit easier. Open up your terminal and type:

`gedit ~/.bashrc`

Add at top of file this line:

`alias cryptutil=/path/to/cryptutil.sh`

(Make sure to enter the full path)

Save and close the file. Now enter command:

`source ~/.bashrc`

Next time when you want to use this tool, you cand simply type in your terminal: `cryptutil` (You can choose another name, if you want).

Also, now you can use `cryptutil -in /path/to/example.com -encrypt -out /path/to/example.com` instead of `./dist/bin/cryptex` from previous example.

Type `cryptutil -help`, for a list of all commands available and their meaning.

### The background stuff.

When you're using the passwords manager, two thing happens in background:

* A daemon process called `controller` starts, and it is responsible for processing your commands (e.g. getting your passwords).

* A second process (also a daemon) called `cryptex` is responsible for cryptographic operations (e.g passwords decryption).

Both of them terminates when you press `Alt+X`.

These hotkeys can be changed in configuration file called `config.json`. Take a look, it's intuitive and easy to do so.

## Authors

* **Romulus-Emanuel Ruja** <<romulus-emanuel.ruja@tutanota.com>>

## License

This project is licensed under the MIT License. Feel free to copy, modify and distribute it - see the [LICENSE](LICENSE) file for details.

## Disclaimer

Keep your master password at safe! Do not use a easy-to-guess master password! Be sure to remember your master password, otherwise your data it's lost! This project cames with no warranty, and you should change settings in `config.json` file only if you understand what you are doing.
