#ifndef _RADIO_HARDWARE_H
#define _RADIO_HARDWARE_H

#include "rftrx.h"

void setupRadios();

// due to the new glitch test data pins cannot be changed!

#if RADIOCOUNT > 0
#define RADIO1ACTIVE

#define RECEIVER1 "433A"
#define RECEIVER1_DATA 19
#define RECEIVER1_VCC 16

#define RECEIVER1_GAIN 21
//#define RECEIVER1_TYPE RX_TYP_AUREL
#endif

#if RADIOCOUNT > 1
#define RADIO2ACTIVE
#define RECEIVER2 "868A"
#define RECEIVER2_DATA 2
#define RECEIVER2_VCC 4
#define RECEIVER2_GAIN 6
//#define RECEIVER2_TYPE RX_TYP_AUREL
#endif

#if RADIOCOUNT > 2
#define RADIO3ACTIVE
#define RECEIVER3 "IR38"
#define RECEIVER3_DATA 18
//#define RECEIVER3_VCC 5
//#define RECEIVER1_TYPE RX_TYP_IR
#endif

#define TRANSMITTER1 "433A"
#define TRANSMITTER1_DATA 14
#define TRANSMITTER1_VCC 15
//#define TRANSMITTER1_TYPE TX_TYP_CHEAP

#define TRANSMITTER3 "IR38"
#define TRANSMITTER3_DATA 17
//#define TRANSMITTER3_VCC 
//#define TRANSMITTER3_TYPE TX_TYP_IR

#endif
