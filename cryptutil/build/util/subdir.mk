OBJECTS += ./util/cleanup.o \
./util/cmd.o \
./util/log.o \
./util/util.o 

CC_DEPS += ./util/deps/cleanup.d \
./util/deps/cmd.d \
./util/deps/log.d \
./util/deps/util.d 

./util/%.o: ../util/%.cc
	@echo 'Building file: $<'
	$(CC) -c -Wall -I../include -I../../../../../../usr/local/include/libcryptography $< -o $@
	@echo 'Build finished: $<'
	@echo

