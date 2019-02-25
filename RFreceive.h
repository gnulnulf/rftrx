/**
 @brief RFreceive library for Arduino
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
 @license  GPLv3. See license.txt
 @details
  This library receives and sends RF,IR signals. No protocol decoding is done here.

  The purpose of this library is to send and recieve data in manchester or spacelength to support all sort of domotic signals. 

 @date 20150107 Initial version
 @date 20151007 join functions with channel selector
 */
 
#ifndef _RFRECEIVE_h
#define _RFRECEIVE_h

// rftrx.h is always included first
#include "rftrx.h"


// if no radiocount is defined the first two radios are expected
#ifndef RADIOCOUNT
  #define RADIOCOUNT 2
#endif

//! a RX buffer can contain max 128 changes/pulse lengths
//! this is mainly limitted by the small amount of memory in an arduino
//#define RX_SIZE 128 
#define RX_SIZE 200 


void appendAndParseFrame(RFframe &f , int channel, long d);
void appendAndParse(String &dest, int channel, long d);
String getReturnstring( RFframe &f );


//! RFreceive contains the interrupt routines for receiving frames
//! it is also used to communicate with the parsing software
class RFreceive {
  public:
    RFreceive();

    //! initialize the interrupt "vector" for RX "channel" 
    void initInterrupt(int vector,int channel);
  



   
    //! enable interrupt for "channel"
    void enableReceive(int channel);
    //! disable interrupt for "channel"
    void disableReceive(int channel);

    //! shared part of the receive interrupt  
    static void receiveInterrupt(int channel) ;

    //! channel 0 routine to attach to the interrupt vector
    static void receiveInterruptChannel0();
    //! channel 1 routine to attach to the interrupt vector
    static void receiveInterruptChannel1();
    //! channel 2 routine to attach to the interrupt vector
    static void receiveInterruptChannel2();
    //! placeholder for old receiver interrupt
    static void receiveInterruptA();

    //! check if data is ready for parsing
    bool dataready(int channel ) ;
    //! get next data
    long getNext(int channel) ;
    //! return datacount
    int dataCount(int channel);

    //! set the minimum startlength
    void setMinStartLength( unsigned long length );
    //! set the maximum startlength
    void setMaxStartLength( unsigned long length );
  
    //! set the minimum periodlength, probably 100+
    void setMinPeriodLength( unsigned long length );
    //! set the maximum periodlength, should be below minstartlength
    void setMaxPeriodLength( unsigned long length );

  private:
static bool pin[RADIOCOUNT];
static signed long filter[ RADIOCOUNT ];
    //! data collecting is active
    static bool activedata[RADIOCOUNT];
    
    //! a glitch is active
    static bool glitch[RADIOCOUNT];

    static unsigned long volatile rxbuffer[RADIOCOUNT][ RX_SIZE +4 ];
    static  unsigned int rxhead[RADIOCOUNT];
    static  unsigned int rxtail[RADIOCOUNT];
    static  unsigned int rxenable[RADIOCOUNT];

    static  unsigned long minStartLength;
    static  unsigned long maxStartLength;

    static   unsigned long minPeriodLength;
    static  unsigned long maxPeriodLength;
  
    //static unsigned long currentTime;
    static unsigned long lastTime[ RADIOCOUNT ];
    //static unsigned long lastTime2;
    //static unsigned long lastTime3;

}; //end RFreceive

#endif
// END RFreceive.h

