###############################################################################
#                     Makefile for Arduino Duemilanove/Uno                    #
#             Copyright (C) 2011 Álvaro Justen <alvaro@justen.eng.br>         #
#                         http://twitter.com/turicas                          #
#                                                                             #
# This project is hosted at GitHub: http://github.com/turicas/arduinoMakefile #
#                                                                             #
# This program is free software; you can redistribute it and/or               #
#  modify it under the terms of the GNU General Public License                #
#  as published by the Free Software Foundation; either version 2             #
#  of the License, or (at your option) any later version.                     #
#                                                                             #
# This program is distributed in the hope that it will be useful,             #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of             #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              #
#  GNU General Public License for more details.                               #
#                                                                             #
# You should have received a copy of the GNU General Public License           #
#  along with this program; if not, please read the license at:               #
#  http://www.gnu.org/licenses/gpl-2.0.html                                   #
###############################################################################

#Sketch, board and IDE path configuration (in general change only this section)
# Sketch filename (should be in the same directory of Makefile)
SKETCH_NAME=cellphone
# The port Arduino is connected
#  Uno, in GNU/linux: generally /dev/ttyACM0
#  Duemilanove, in GNU/linux: generally /dev/ttyUSB0
PORT=/dev/tty.usbmodem1421
# The path of Arduino IDE
ARDUINO_DIR=$(HOME)/arduino-1.0.5
# Boardy type: use "arduino" for Uno or "stk500v1" for Duemilanove
BOARD_TYPE=wiring
# Baud-rate: use "115200" for Uno or "19200" for Duemilanove
BAUD_RATE=115200

#Compiler and uploader configuration
ARDUINO_CORE=$(ARDUINO_DIR)/hardware/arduino/cores/arduino
ARDUINO_SKETCHBOOK=$(HOME)/sketchbook
INCLUDE=-I. -I$(ARDUINO_DIR)/hardware/arduino/cores/arduino \
		-I$(ARDUINO_DIR)/hardware/arduino/variants/mega -I$(ARDUINO_SKETCHBOOK)/libraries/Keypad

TMP_DIR=./build_arduino
MCU=atmega2560
DF_CPU=16000000
CC=avr-gcc
CPP=avr-g++
AVR_OBJCOPY=avr-objcopy 
AVRDUDE=avrdude
CC_FLAGS=-g -Os -w -Wall -ffunction-sections -fdata-sections -fno-exceptions \
	 -std=gnu99
CPP_FLAGS=-g -Os -w -Wall -ffunction-sections -fdata-sections -fno-exceptions
AVRDUDE_CONF=$(ARDUINO_DIR)/hardware/tools/avrdude.conf
#CORE_C_FILES=pins_arduino WInterrupts wiring_analog wiring wiring_digital
CORE_C_FILES=WInterrupts wiring_analog wiring wiring_digital \
	     wiring_pulse wiring_shift
CORE_CPP_FILES=HardwareSerial main Print Tone WMath WString
SKETCH_SRC=$(wildcard *.cpp)

# Local resources
LOCAL_C_SRCS    ?= $(wildcard *.c)
LOCAL_CPP_SRCS  ?= $(wildcard *.cpp)
LOCAL_SRCS      = $(LOCAL_C_SRCS) $(LOCAL_CPP_SRCS)

ifndef ARDUINO_LIBS 
		# automatically determine included libraries
		ARDUINO_LIBS += $(filter $(notdir $(wildcard $(ARDUINO_DIR)/libraries/*)), \
						$(shell sed -ne "s/^ *\# *include *[<\"]\(.*\)\.h[>\"]/\1/p" $(LOCAL_SRCS)))
		ARDUINO_LIBS += $(filter $(notdir $(wildcard $(ARDUINO_SKETCHBOOK)/libraries/*)), \
						$(shell sed -ne "s/^ *\# *include *[<\"]\(.*\)\.h[>\"]/\1/p" $(LOCAL_SRCS)))
endif
USER_LIB_PATH = $(ARDUINO_SKETCHBOOK)/libraries
USER_LIBS     = $(wildcard $(patsubst %,$(USER_LIB_PATH)/%,$(ARDUINO_LIBS)))
USER_LIB_NAMES= $(patsubst $(USER_LIB_PATH)/%,%,$(USER_LIBS))


OBJDIR=$(TMP_DIR)
USER_LIBS    := $(wildcard $(USER_LIBS) $(addsuffix /utility,$(USER_LIBS)))
USER_LIBS_INCLUDE = $(patsubst %,-I%,$(USER_LIBS))
USER_LIB_CPP_SRCS   = $(wildcard $(patsubst %,%/*.cpp,$(USER_LIBS)))
#USER_LIB_C_SRCS     = $(wildcard $(patsubst %,%/*.c,$(USER_LIBS)))
USER_LIB_C_SRCS     = 
USER_LIB_OBJS = $(patsubst $(USER_LIB_PATH)/%.cpp,$(OBJDIR)/libs/%.o,$(USER_LIB_CPP_SRCS)) \
				$(patsubst $(USER_LIB_PATH)/%.c,$(OBJDIR)/libs/%.o,$(USER_LIB_C_SRCS))
USER_LIB_SRCS = $(USER_LIB_CPP_SRCS) $(USER_LIB_C_SRCS)

TMP_OBJS =$(sort $(addprefix $(TMP_DIR)/,$(notdir $(USER_LIB_OBJS))))
LIB_SRCS =$(sort $(USER_LIB_SRCS))

all: clean compile

.PHONEY:clean
clean:
	@echo '# *** Cleaning...'
	rm -rf "$(TMP_DIR)"

compile: sketch_file arduino_core user_libs
		@echo '# *** Compiling...'

		@#mkdir $(TMP_DIR)
		@#echo '#include "Arduino.h"' > "$(TMP_DIR)/$(SKETCH_NAME).c"
		@#cat $(SKETCH_NAME) >> "$(TMP_DIR)/$(SKETCH_NAME).c"

		@#$(CPP) -MM -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) \
		#         $(CPP_FLAGS) "$(TMP_DIR)/$(SKETCH_NAME).cpp" \
		#	 -MF "$(TMP_DIR)/$(SKETCH_NAME).d" \
		#	 -MT "$(TMP_DIR)/$(SKETCH_NAME).o"

		@#Compiling the sketch file:
		#for pro_c_file in $(SKETCH_SRC); do \
		#$(CPP) -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) \
		#       $(CPP_FLAGS) -c $$pro_c_file \
		#       -o $(TMP_DIR)/$$pro_c_file.o; \
		#done

		@#Compiling Arduino core .c dependecies:
		#for core_c_file in ${CORE_C_FILES}; do \
		#    $(CC) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) \
		#          $(CC_FLAGS) $(ARDUINO_CORE)/$$core_c_file.c \
		#	  -o $(TMP_DIR)/$$core_c_file.o; \
		#done

		@#Compiling Arduino core .cpp dependecies:
		@#for core_cpp_file in ${CORE_CPP_FILES}; do \
		#    $(CPP) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) \
		#           $(CPP_FLAGS) $(ARDUINO_CORE)/$$core_cpp_file.cpp \
		#	   -o $(TMP_DIR)/$$core_cpp_file.o; \
		#done

		@#TODO: compile external libraries here
		@#TODO: use .d files to track dependencies and compile them
		@#      change .c by -MM and use -MF to generate .d
		
		$(CC) -mmcu=$(MCU) -lm -Wl,--gc-sections -Os \
		      -o $(TMP_DIR)/$(SKETCH_NAME).elf $(TMP_DIR)/*.o
		$(AVR_OBJCOPY) -O ihex -R .eeprom \
		               $(TMP_DIR)/$(SKETCH_NAME).elf \
			       $(TMP_DIR)/$(SKETCH_NAME).hex
		@echo '# *** Compiled successfully! \o/'

sketch_file:
	mkdir $(TMP_DIR)

	@echo 'Compiling the sketch file:';
	for pro_c_file in $(SKETCH_SRC); do \
		$(CPP) -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) \
		$(CPP_FLAGS) -c $$pro_c_file \
		-o $(TMP_DIR)/$$pro_c_file.o; \
	done

arduino_core:
	@#Compiling Arduino core .c dependecies:
	for core_c_file in ${CORE_C_FILES}; do \
		$(CC) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) \
		$(CC_FLAGS) $(ARDUINO_CORE)/$$core_c_file.c \
		-o $(TMP_DIR)/$$core_c_file.o; \
	done

	@#Compiling Arduino core .cpp dependecies:
	for core_cpp_file in ${CORE_CPP_FILES}; do \
		$(CPP) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) \
		$(CPP_FLAGS) $(ARDUINO_CORE)/$$core_cpp_file.cpp \
		-o $(TMP_DIR)/$$core_cpp_file.o; \
	done

user_libs: $(TMP_OBJS)

#$(USER_LIB_OBJS):$(USER_LIB_SRCS)
#$(TMP_OBJS): $(USER_LIB_SRCS)
$(TMP_OBJS): $(LIB_SRCS)
	#Compiling user libraries
	echo $(TMP_OBJS)
	echo $(USER_LIB_SRCS)
	@#$(CPP) -c -mmcu=$(MCU) $(INCLUDE) $(USER_LIBS_INCLUDE) -DARDUINO=150 $(CPP_FLAGS) $< -o $@
	@#$(CPP) -mmcu=$(MCU) $(INCLUDE) $(USER_LIBS_INCLUDE) -DARDUINO=150 $(CPP_FLAGS) -c $^

	$(CPP) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(USER_LIBS_INCLUDE) -DARDUINO=150 \
		$(CPP_FLAGS) $(ARDUINO_SKETCHBOOK)/libraries/Keypad/Keypad.cpp \
		-o $(TMP_DIR)/Keypad.o

	$(CPP) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(USER_LIBS_INCLUDE) -DARDUINO=150 \
		$(CPP_FLAGS) $(ARDUINO_SKETCHBOOK)/libraries/Keypad/utility/Key.cpp \
		-o $(TMP_DIR)/Key.o



reset:
		@echo '# *** Resetting...'
		stty --file $(PORT) hupcl
		sleep 0.1
		stty --file $(PORT) -hupcl
		

upload:
		@echo '# *** Uploading...'
		$(AVRDUDE) -V -v -v -v -p $(MCU) -C $(AVRDUDE_CONF) -c $(BOARD_TYPE) \
		           -b $(BAUD_RATE) -P $(PORT) \
				   -D -U flash:w:$(TMP_DIR)/$(SKETCH_NAME).hex:i
		@echo '# *** Done - enjoy your sketch!'
