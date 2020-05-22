/*
 * decode frame library v1.0 for Arduino
 *
 * decodes a binairy string to the different hardware data
 *
 * Copyright 2015 by Arco van Geest <arco@appeltaart.mine.nu>
 *
 * The purpose of this library is to send and recieve data in manchester or spacelength to support all sort of domotic signals.
 *
 * 20151011 Initial version
 *
 * License: GPLv3. See license.txt
 */

#ifndef _FRAME_DECODE_H
#define _FRAME_DECODE_H

#include "rftrx.h"

#include <Arduino.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>


#define FRAMESIZE 520
//#define FRAMESIZE (RX_SIZE/2)
#define MIN_FRAMESIZE 24
#define SPACELEN_TOLERANCE 5

#define PROTO_SPACELEN 1
#define PROTO_1214 2
#define PROTO_1331 3
#define PROTO_TRITS 4
#define PROTO_NRZ 5
#define PROTO_NRZI 6
#define PROTO_RZUNI 7
#define PROTO_MANCHESTER 8


#define PROTO_COUNT 8


//array min_period, max_period, count, max_count, array of matches, &handler 
// 140,190,49,52,["-1-1-1-1-1..."],&impuls_rx


int frame_decode(String &frame);
int frame_decode(RFframe &frame);


int decode_1331( String &spacelenframe );

#endif
