#Set the path where the SDK is stored
SDK_DIR=./
SDK_INC_DIR=$(SDK_DIR)/Include
SDK_LIB_DIR=$(SDK_DIR)/Bins/Platforms
EXEC=NanoPlayer
EXTRA_LINK=nothing_to_do
##########################
# WINDOWS Specific BUILD #
##########################
ifeq ($(OS),Windows_NT)
ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
$(info $************  WINDOWS 64 Bits  ************)
endif
ifeq ($(PROCESSOR_ARCHITECTURE),x86)
$(info $************  WINDOWS 32 Bits  ************)
endif
else
UNAME_S := $(shell uname -s)
########################
# LINUX Specific BUILD #
########################
ifeq ($(UNAME_S),Linux)
$(info $************  Linux  ************)
SDK_LIB_DIR:=$(addsuffix /Linux,$(SDK_LIB_DIR))
CC=gcc
CFLAGS=-W -Wall -I$(SDK_INC_DIR) -Wno-unused-parameter
LDFLAGS=-L$(SDK_LIB_DIR) \
        -Wl,-rpath $(SDK_LIB_DIR) \
        -ldeezer
UNAME_P := $(shell uname -p)
ifeq ($(UNAME_P),x86_64)
$(info $************  64 Bits  ************)
SDK_LIB_DIR:=$(addsuffix /x86_64,$(SDK_LIB_DIR))
endif
ifneq ($(filter %86,$(UNAME_P)),)
$(info $************  32 Bits  ************)
SDK_LIB_DIR:=$(addsuffix /i386,$(SDK_LIB_DIR))
endif
ifneq ($(filter arm%,$(UNAME_P)),)
$(info $************  ARM  ************)
SDK_LIB_DIR:=$(addsuffix /arm,$(SDK_LIB_DIR))
endif
endif # fi linux
###########################
#  MAC OS Specific BUILD  #
###########################
ifeq ($(UNAME_S),Darwin)
$(info $************  MacOS  ************)
SDK_LIB_DIR:=$(addsuffix /MacOSX,$(SDK_LIB_DIR))
CC=clang
CFLAGS=-W -Wall -I$(SDK_INC_DIR) -Wno-unused-parameter

LDFLAGS=-framework SystemConfiguration \
        -framework Security \
        -framework CoreFoundation \
        -framework CoreServices \
        -framework CoreAudio \
        -framework AudioUnit \
        -framework AudioToolbox \
        -framework IOKit \
        -lstdc++ \
        -framework libdeezer \
        -iframework $(SDK_LIB_DIR)
#On Mac OS we need to force the path where the libdeezer framework is
#or need to copy libdeezer framework in /Library/Frameworks/
#or /System/Library/Frameworks/
#or @executable_path/../Frameworks/libdeezer.framework/Versions/A/libdeezer
EXTRA_LINK=install_name_tool \
           -change \
           @executable_path/../Frameworks/libdeezer.framework/Versions/A/libdeezer \
           $(SDK_LIB_DIR)/libdeezer.framework/Versions/A/libdeezer \
           $(EXEC)
endif #fi Darwin

endif

all: $(EXEC)

NanoPlayer: main.o
	$(CC) -o $@ $^ $(LDFLAGS)
ifneq ($(EXTRA_LINK),nothing_to_do)
	$(EXTRA_LINK)
endif

# Build every source files
%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

# Clean every temporary files
clean:
	rm -rf *.o

# Clean everything than can be generated
mrproper: clean
	rm -rf $(EXEC)
