CC=g++
TARGET=./dist/bin/cryptex
CC_DEPS :=
OBJECTS :=
RM=rm -v

all: ./dist/bin/cryptex

-include $(CC_DEPS)
-include ./cryptex/src/subdir.mk

./dist/bin/cryptex: $(OBJECTS)
	$(CC) $(OBJECTS) -lcrypto ./cryptography/libcryptography.so.* -o $(TARGET)

clean:
	$(RM) $(OBJECTS) $(TARGET)

config:
	python3 ./scripts/config.py -defaults -out config.json && cd cryptography && make

setup:
	python3 ./scripts/setup.py

