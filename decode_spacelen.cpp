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
// 
long gettritsbinfromframe( char c, int val0, int val1, int val2, int val3 ,String &frame, String &dataFrame, int offset) {
        long ret=0;
int val[]={ val0, val1, val2, val3 };
        for (int i=0;i< dataFrame.length();i+=2) {
                if ( dataFrame.charAt( i ) == c ) {
                        ret<<=1;
			int dum =0;
                        dum |= ( frame.charAt( i + offset ) == '1' ) ?1:0;
                        dum<<=1;
			dum |= ( frame.charAt( i + offset +1 ) == '1' ) ?1:0;
                        ret |=  val[ dum];
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
	int decoded=0;


	
	if ( framelen == 21 ) {
		Serial.print(F("ATIremote:"));
		/* from the kernel source:
		 * Each remote can be configured to transmit on one channel as follows:
		 *   - Press and hold the "hand icon" button.  
		 *   - When the red LED starts to blink, let go of the "hand icon" button. 
		 *   - When it stops blinking, input the channel code as two digits, from 01 
		 *     to 16, and press the hand icon again.
		*/
		String match=F("---------------------");
		// f = first transmission (0=repeat)
		// k = keycode
		// i = channel/index
		String data=F( "fkkkkkkkkkkkkkkkkiiii");
                Serial.print("IDX=");
                Serial.print( getbinfromframe( 'i', spacelenFrame, data,0));
                Serial.print(";KEY=");
                Serial.print( getbinfromframe( 'k', spacelenFrame, data,0),HEX);

                Serial.print(";FIRST=");
                Serial.print( getbinfromframe( 'f', spacelenFrame, data,0));
                Serial.print(";frame=");
                Serial.print( spacelenFrame);
                Serial.println();
		decoded=0;
	} //21


	if ( framelen == 32 ) {
		Serial.print(F("Auriol:"));
//#        echo SPACELEN weather Auriol
  //      # IIII IIII UUUU TTTT TTTT TTTT  CCCC CCCC
   //     # index     bat? temp            checksum?
		String data=F("iiiiiiiiuuuuttttttttttttcccccccc");
                Serial.print("IDX=");
                Serial.print( getbinfromframe( 'i', spacelenFrame, data,0));
                Serial.print(";TEMP=");
                Serial.print( getsignedbinfromframe( 't', spacelenFrame, data,0));
                Serial.print(";UNKNOWN=");
                Serial.print( getbinfromframe( 'u', spacelenFrame, data,0),BIN);
                Serial.print(";CHECKSUM=");
                Serial.print( getbinfromframe( 'c', spacelenFrame, data,0));
                Serial.println();



}

        if ( framelen == 34 ) {
                Serial.print(F("TempHum:"));
                String match=F("---------tt----------------------");
                String data=F("iiiiiibbcctttttttttttteeeeehhhhhhh");
                Serial.print(F("IDX="));
                Serial.print( getbinfromframe( 'i', spacelenFrame, data,0));
                Serial.print(F(";TEMP="));
                Serial.print( getsignedbinfromframe( 't', spacelenFrame, data,0));
                Serial.print(F(";HUM="));
                Serial.print( getbinfromframe( 'h', spacelenFrame, data,0));
                Serial.print(F(";BAT="));
                Serial.print( getbinfromframe( 'b', spacelenFrame, data,0));
                Serial.print(F(";CHAN="));
                Serial.print( getbinfromframe( 'c', spacelenFrame, data,0));
                Serial.println();

                decoded=0;
        }       //34

	if ( framelen == 36 ) {
		Serial.print(F("TempHum:"));
		String match=F("-----------tt----------------------");
		String data=F("--iiiiiibbcctttttttttttteeeeehhhhhhh");
		Serial.print(F("IDX="));
		Serial.print( getbinfromframe( 'i', spacelenFrame, data,0));
		Serial.print(F(";TEMP="));
		Serial.print( getsignedbinfromframe( 't', spacelenFrame, data,0));
		Serial.print(F(";HUM="));
		Serial.print( getbinfromframe( 'h', spacelenFrame, data,0));
		Serial.print(F(";BAT="));
		Serial.print( getbinfromframe( 'b', spacelenFrame, data,0));
		Serial.print(F(";CHAN="));
		Serial.print( getbinfromframe( 'c', spacelenFrame, data,0));
		Serial.println();

		decoded=0;
	}	//36 
	if ( framelen == 64 ) {
/* newkaku, zonder dimmer

- 26 bit:  Address
- 1  bit:  group bit
- 1  bit:  on/off/[dim]
- 4  bit:  unit
- [4 bit:  dim level. Only present of [dim] is chosen]
newkaku 00FBC8B2	10

0085856E	3 iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiggoouuuuuuuu 
>SPACELENFRAME:64:0101100101010110011010010101011001100110100110101001010101011001
>SPACELENFRAME:64:01 01 10 01 01 01 01 10 01 10 10 01 01 01 01 10 01 10 01 10 10 01 10 10 10 01 01 01 01 01 10 01
                  00 1000 0101 1000 0101  0110 1110  0  0  0010
                   0    8    5    8    5     6    e  G  O     2(ch 3)
*/
		String match1=F("iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii10oouuuuuuuu");
		String match2=F("iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii01oouuuuuuuu");
		String data=F("iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiggoouuuuuuuu");
		Serial.print("Newkaku:");
		Serial.print("IDX=");
                Serial.print( gettritsbinfromframe( 'i',0,0,1,0, spacelenFrame, data,0),HEX);
		Serial.print(" GROUP=");
                Serial.print( gettritsbinfromframe( 'g',0,0,1,0, spacelenFrame, data,0));
		Serial.print(" STATE=");
                Serial.print( gettritsbinfromframe( 'o',0,0,1,0, spacelenFrame, data,0));
		Serial.print(" UNIT=");
                Serial.print( gettritsbinfromframe( 'u',0,0,1,0, spacelenFrame, data,0) +1);
                Serial.println();

		//decoded=1;

} //64

        if ( framelen == 72 ) {
/* newkaku, zonder dimmer

- 26 bit:  Address
- 1  bit:  group bit
- 1  bit:  on/off/[dim]
- 4  bit:  unit
- [4 bit:  dim level. Only present of [dim] is chosen]
newkaku 00FBC8B2        10

0085856E        3
>SPACELENFRAME:64:0101100101010110011010010101011001100110100110101001010101011001
>SPACELENFRAME:64:01 01 10 01 01 01 01 10 01 10 10 01 01 01 01 10 01 10 01 10 10 01 10 10 10 01 01 01 01 01 10 01
                  00 1000 0101 1000 0101  0110 1110  0  0  0010
                   0    8    5    8    5     6    e  G  O     2(ch 3)
*/
		String match=F("----------------------------------------------------11------------------");
		String data=F("iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiggoouuuuuuuu");
                Serial.print(F("NewkakuDim:"));
		Serial.print(F("IDX="));
                Serial.print( gettritsbinfromframe( 'i',0,0,1,0, spacelenFrame, data,0),HEX);
		Serial.print(F(" GROUP="));
                Serial.print( gettritsbinfromframe( 'g',0,0,1,0, spacelenFrame, data,0));
		Serial.print(F(" STATE=DIM"));
		Serial.print(F(" UNIT="));
                Serial.print( gettritsbinfromframe( 'u',0,0,1,0, spacelenFrame, data,0) +1);
		Serial.print(F(" DIM="));
                Serial.print( gettritsbinfromframe( 'd',0,0,1,0, spacelenFrame, data,0) );
                Serial.println();

                decoded=0;

} //72


	if ( framelen == 88 ) {
		Serial.print(F("Weather:"));
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
		String data=F("--------iiiiiiiiiiiitttttttttttthhhhhhhhwwwwwwwwggggggggmmmmrrrrssssssssbbbbddddcccccccc");
		                Serial.print(F("IDX="));
                Serial.print( getbinfromframe( 'i', spacelenFrame, data,0));
                Serial.print(F(";TEMP="));
                Serial.print( getbinfromframeinverted( 't', spacelenFrame, data,0) -400);
                Serial.print(F(";HUM="));
                Serial.print( getbinfromframeinverted( 'h', spacelenFrame, data,0));
                Serial.print(F(";BAT="));
                Serial.print( getbinfromframeinverted( 'b', spacelenFrame, data,0));
                Serial.print(F(";WIND="));
                Serial.print( getbinfromframeinverted( 'g', spacelenFrame, data,0));
                Serial.println();

		decoded=0;
	} //88	





return decoded;
}




