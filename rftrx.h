#ifndef _RFTRX_H
#define _RFTRX_H

#define BVERSION "20160507"
#define RADIOCOUNT 2 



// Arduino base
#include <Arduino.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>


struct RFconfig {
  // 0=off, 1=allways, 2 only on decode failure
  int showRAW=0;
  bool debugProto=false;
  
};

typedef struct RFframe {
  String receiver;
  int count;
  int period;
  String data;
  String returnstring;
  String output[];
} RFframe;




// RFTRX 
#include "radio_hardware.h"
#include "SerialCommands.h"
#include "commands.h"
#include "RFreceive.h"
#include "RFtransmit.h"

#include "decode_spacelen.h"
#include "frame_decode.h"
//#include "Regexp.h"
#include "tools.h"


extern String dum;

extern RFreceive myrx;


#endif

