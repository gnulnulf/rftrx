/*
 * decode spacelen library v1.0 for Arduino
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

#ifndef _SPACELEN_DECODE_H
#define _SPACELEN_DECODE_H

#include <Arduino.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>


int decode_spacelen( String &spacelenframe );
long getbinfromframe( char c, String &frame, String &dataFrame, int offset=0) ;



#endif

