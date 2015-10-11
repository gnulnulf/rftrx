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

#include "decode_spacelen.h"

long getsignedbinfromframe( char c, String &frame, String &dataFrame, int offset) {
        long ret=0;
	int bitcount=0;
        for (int i=0;i< dataFrame.length();i++) {
                if ( dataFrame.charAt( i ) == c ) {
			bitcount++;
                        ret<<=1;
                        ret |= ( frame.charAt( i + offset ) == '1' ) ?1:0;
                }

        }
	/*	Serial.print("(BITCOUNT=");
		Serial.print( bitcount );
		Serial.print(")");

		Serial.print("(RETPRE=");
		Serial.print( ret, BIN );
		Serial.print(")");

		Serial.print("(HIGHBIT=");
		Serial.print( ret & (1<<(bitcount-1)), BIN );
		Serial.print(")");
*/

	// test if the 12th bit is one and or the missing bits.
	ret = (ret >> (bitcount -1)) == 0 ? ret : -1 ^ ( (1<<(bitcount-1))-1 )  | ret;

//		Serial.print("(RETPOST=");
//		Serial.print( ret, BIN );
//		Serial.print(")");
		
        return ret;

}

long getbinfromframe( char c, String &frame, String &dataFrame, int offset) {
	long ret=0;
	for (int i=0;i< dataFrame.length();i++) {
		if ( dataFrame.charAt( i ) == c ) {
			ret<<=1;
			ret |= ( frame.charAt( i + offset ) == '1' ) ?1:0;
		}

	}
	return ret;
}

long getbinfromframeinverted( char c, String &frame, String &dataFrame, int offset) {
        long ret=0;
        for (int i=0;i< dataFrame.length();i++) {
                if ( dataFrame.charAt( i ) == c ) {
                        ret<<=1;
                        ret |= ( frame.charAt( i + offset ) == '0' ) ?1:0;
                }

        }
        return ret;
}


int decode_spacelen( String &spacelenFrame ) {
	int framelen = spacelenFrame.length() ;
	if ( framelen == 36 ) {
		Serial.print("TempHum:");
		String match="-----------tt----------------------";
		String data="iiiiiiiibbcctttttttttttteeeeehhhhhhh";
		Serial.print("IDX=");
		Serial.print( getbinfromframe( 'i', spacelenFrame, data,0));
		Serial.print(";TEMP=");
		Serial.print( getsignedbinfromframe( 't', spacelenFrame, data,0));
		Serial.print(";HUM=");
		Serial.print( getbinfromframe( 'h', spacelenFrame, data,0));
		Serial.print(";BAT=");
		Serial.print( getbinfromframe( 'b', spacelenFrame, data,0));
		Serial.print(";CHAN=");
		Serial.print( getbinfromframe( 'c', spacelenFrame, data,0));
		Serial.println();

	}	
	if ( framelen == 88 ) {
		Serial.println("Weather");
/*
#    abc: device identifier
#    def: temperature
#    gh: humidity
#    ij: average wind speed low byte
#    kl: gust wind speed low byte
#    m: unknown
#    n: rainfall counter high nibble
#    op: rainfall counter
#    q: battery-low indicator
#    r: wind direction
#    st: checksum
--------aaaabbbbccccddddeeeeffffgggghhhhiiiijjjjkkkkllllmmmmnnnnooooppppqqqqrrrrsssstttt
--------iiiiiiiiiiiitttttttttttthhhhhhhhwwwwwwwwggggggggmmmmrrrrssssssssbbbbddddcccccccc
01111111101010111110001001010100 596-400 / 10 = 19,6
1000000001010100000111011010101111000001111111111111111111111111101011101111001101011010
*/
		String data="--------iiiiiiiiiiiitttttttttttthhhhhhhhwwwwwwwwggggggggmmmmrrrrssssssssbbbbddddcccccccc";
		                Serial.print("IDX=");
                Serial.print( getbinfromframe( 'i', spacelenFrame, data,0));
                Serial.print(";TEMP=");
                Serial.print( getbinfromframeinverted( 't', spacelenFrame, data,0) -400);
                Serial.print(";HUM=");
                Serial.print( getbinfromframeinverted( 'h', spacelenFrame, data,0));
                Serial.print(";BAT=");
                Serial.print( getbinfromframeinverted( 'b', spacelenFrame, data,0));
                Serial.print(";WIND=");
                Serial.print( getbinfromframeinverted( 'g', spacelenFrame, data,0));
                Serial.println();

	}	
}




