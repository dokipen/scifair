ARDMK_DIR = ${HOME}/src/Arduino-Makefile
BOARD_TAG = mega
ARDUINO_DIR = arduino-1.0.1
ARDUINO_PORT = /dev/ttyACM0
ARDUINO_TOOLS_DIR = ${ARDUINO_DIR}/hardware/tools
AVRDUDE_ARD_PROGRAMMER = stk500v2
AVRDUDE_ARD_BAUDRATE = 9600
AVRDUDE_CONF = ${ARDUINO_TOOLS_DIR}/avrdude.conf
AVRDUDE = ${ARDUINO_TOOLS_DIR}/avrdude
MONITOR_CMD = screen -RRad scifair
AVR_TOOLS_DIR = ${ARDUINO_TOOLS_DIR}/avr
MCU = atmega2560
VARIANT = mega
F_CPU = 16000000

unmonitor:
		- pkill -c -f "SCREEN -RRad scifair"

upload : unmonitor

include ${ARDMK_DIR}/arduino-mk/Arduino.mk
