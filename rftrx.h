/**
 @brief RFTRX scanner for Arduino
 @version 1.0
 @author Arco van Geest <arco@appeltaart.mine.nu>
 @copyright 2015-2018 Arco van Geest <arco@appeltaart.mine.nu> All right reserved.
  rftrx is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  rftrx is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with rftrx.  If not, see <http://www.gnu.org/licenses/>.

 @details
  Up to three radios can be connected
  Board         int.0 int.1 int.2 int.3 int.4 int.5
  Uno, Ethernet 2     3
  Mega2560      2     3     21    20    19    18

  The receiver could be connected to pin2 (int0)
  The receiver could be connected to pin19 (int4)
  The receiver could be connected to pin18 (int5)

*/
#ifndef _RFTRX_H
#define _RFTRX_H

#define BVERSION "20200305"
#define RADIOCOUNT 1


#define RFLINK

// Arduino base
#include <Arduino.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//! Receive configuration
struct RFconfig {
  // 0=off, 1=allways, 2 only on decode failure
  int showRAW=0;
  bool debugProto=false;

};

//! structure of a frame
typedef struct RFframe {
  String receiver;    //!< reciver hardware string
  int count;          //!< pulse count
  int period;         //!< single period time
  long startperiod;   //!< length of start period
  String data;        //!< string representation of fram
  String reldata;
  String returnstring;
  String output[];
  unsigned long crc;
} RFframe;




// RFTRX
#include "radio_hardware.h"

#include "commands.h"
#include "RFreceive.h"
#include "RFtransmit.h"

#include "decode_spacelen.h"
#include "frame_decode.h"
#include "frame_encode.h"
//#include "Regexp.h"
#include "tools.h"
#include "SerialCommands.h"

extern String dum;

extern RFreceive myrx;


#endif
