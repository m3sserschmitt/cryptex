OBJECTS += ./core/cryptex.o \
./core/daemon.o 

CC_DEPS += ./core/deps/cryptex.d \
./core/deps/daemon.d 

./core/%.o: ../core/%.cc
	@echo 'Building file: $<'
	$(CC) -c -Wall -I../include -I../../../../../../usr/local/include/libcryptography $< -o $@
	@echo 'Build finished: $<'
	@echo

