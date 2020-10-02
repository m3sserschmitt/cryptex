# cryptex

Password manager & cryptography tool.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine.

### Prerequisites

You need OpenSSL library to be installed on your machine. Type this command in your terminal:

`openssl version`

If you see something similar to this: `OpenSSL 1.1.1g  21 Apr 2020`, it means that you have OpenSSL installed, otherwise you have to install the library in order to compile this project. Checkout [OpenSSL](https://www.openssl.org/) site for details.

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

After the building process done,