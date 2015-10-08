#BOARD_TAG = uno
#ARDUINO_PORT = /dev/ttyACM0
#RECEIVER = \"868A\"

BOARD_TAG = mega2560
ARDUINO_PORT = /dev/ttyUSB0
RECEIVER = \"433A\"


BVERSION=`date +'%Y%m%d'`

#CPPFLAGS     = -DBOARD_TAG="$(BOARD_TAG)" -DBVERSION="$(BVERSION)"
#CPPFLAGS = -DBVERSION=$(BVERSION) -DRECEIVER=$(RECEIVER)


ARDUINO_LIBS = RFtrx
ARDUINO_DIR = /usr/share/arduino
include /usr/share/arduino/Arduino.mk
