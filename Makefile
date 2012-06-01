ARDMK_DIR = ${HOME}/src/Arduino-Makefile
BOARD_TAG = mega
ARDUINO_DIR = /home/doki_pen/opt/arduino-1.0.1
ARDUINO_PORT = /dev/ttyACM0
AVRDUDE_ARD_PROGRAMMER = stk500v2
AVRDUDE_ARD_BAUDRATE = 9600
AVRDUDE_CONF = ${ARDUINO_DIR}/hardware/tools/avrdude.conf
MONITOR_CMD = screen -RRad scifair
AVR_TOOLS_DIR = ${ARDUINO_DIR}/hardware/tools/avr
MCU = atmega2560
VARIANT = mega
F_CPU = 16000000

include ${ARDMK_DIR}/arduino-mk/Arduino.mk

unmonitor:
		pkill -f "SCREEN -RRad scifair"
