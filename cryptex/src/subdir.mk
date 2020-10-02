OBJECTS += ./cryptex/src/cleanup.o\
./cryptex/src/cmd.o\
./cryptex/src/cryptex.o\
./cryptex/src/daemon.o\
./cryptex/src/log.o\
./cryptex/src/main.o\
./cryptex/src/util.o

CC_DEPS += ./cryptex/src/deps/cleanup.d\
./cryptex/src/deps/cmd.d\
./cryptex/src/deps/cryptex.d\
./cryptex/src/deps/daemon.d\
./cryptex/src/deps/log.d\
./cryptex/src/deps/main.d\
./cryptex/src/deps/util.d

./cryptex/src/%.o: ./cryptex/src/%.cc
	$(CC) -I./cryptography/include -Wall -c $< -o $@

