/*
 * RFreceive library v1.0 for Arduino
 * 
 * This library receives and sends RF,IR signals. No protocol decoding is done here.
 * 
 * Copyright 2015 by Arco van Geest <arco@appeltaart.mine.nu>
 *
 * The purpose of this library is to send and recieve data in manchester or spacelength to support all sort of domotic signals. 
 *
 * 20150107 Initial version
 * 20151007 join functions with channel selector
 * 
 * License: GPLv3. See license.txt
 */
 
#ifndef _RFRECEIVE_h
#define _RFRECEIVE_h

#include "rftrx.h"



#ifndef RADIOCOUNT
#define RADIOCOUNT 2
#endif

// buffer can contain max 128 changes/pulse lengths
//#define RX_SIZE 128 
#define RX_SIZE 200 


void appendAndParseFrame(RFframe &f , int channel, long d);
void appendAndParse(String &dest, int channel, long d);
String getReturnstring( RFframe &f );



class RFreceive {
  public:
    RFreceive();

  void initInterrupt(int vector,int channel);
  
  
  void enableReceive(int channel);
   void disableReceive(int channel);
   static void receiveInterrupt(int channel) ;

static void receiveInterruptChannel0();
static void receiveInterruptChannel1();
static void receiveInterruptChannel2();
static void receiveInterruptA();


   bool dataready(int channel ) ;
   long getNext(int channel) ;
  int dataCount(int channel);
   
   void setMinStartLength( unsigned long length );
   void setMaxStartLength( unsigned long length );
  
   void setMinPeriodLength( unsigned long length );
   void setMaxPeriodLength( unsigned long length );
  
  
  


private:
static bool activedata[RADIOCOUNT];
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

};



#endif

