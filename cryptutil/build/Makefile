CC :=g++
LD :=g++
TARGET :=cryptutil
OUT :=./cryptutil
LIBS :=-lcryptography
CC_DEPS :=
OBJECTS :=
RM :=rm -v

all: $(OUT)

-include ./core/subdir.mk
-include ./subdir.mk
-include ./util/subdir.mk
-include $(CC_DEPS)

$(OUT): $(OBJECTS)
	@echo Building target: "$@".
	@echo Invoking $(LD) Linker ...
	$(LD) $(OBJECTS) $(LIBS)  -o $(OUT)
	@echo Target $(TARGET) build successfully.
	@echo Done.

clean:
	$(RM) $(OBJECTS) ./cryptutil

install:
	cp ./cryptutil /usr/local/bin/cryptutil

