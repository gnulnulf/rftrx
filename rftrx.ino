/*
 * RFTRX scanner v1.0 for Arduino
 *
 * Example to view received frames
 *
 * Copyright 2015 by Arco van Geest <arco@appeltaart.mine.nu>
 *
 * The receiver should be connected to pin2 (int0)
 *
 * 20150107 Initial version
 * 20151004 triple receiver
 *
 * License: GPLv3. See license.txt
 */

/*
 * get config from eeprom
 * setup hardware
 * setup receive
 *
 * RFreceive frame
 * - frame_decode
 * -- find protocol
 * --- parse protocols
 * ---- match hardwre
 * ----- decode hardware
 *
 * send_hardware
 * - hardware to bits
 * -- bits to protocol
 * --- protocol to relstring
 * ---- send relstring
 *
 * serial commands
 *
 */
#include "rftrx.h"
//String dum;

String returnstr;

void setup() {
  // initialize serial port
  serialInit();
  // show version
  cmd_version("");
  // show configured radios
  cmd_showradios("");

  // setup radio hardware
  setupRadios();

  // setup transmiters
  setupTX();

  // enable first receiver
  myrx.enableReceive( 0 );
  myrx.enableReceive( 1 );

  // enable second receiver
  //  myrx.enableReceive( 1 );

  // enable third receiver
  //  myrx.enableReceive( 2);

  //myrx.disableReceive( 0 );
 // myrx.disableReceive( 1 );

 // send a frame
   cmd_txframe("");
} //setup


/*
 * LOOP
 */



#define CACHESIZE 30
#define CACHEDELAY 1000
long cache[RADIOCOUNT][CACHESIZE];
int cachecount[RADIOCOUNT][CACHESIZE];
void loop() {

    RFframe frame;



//  int col=0;

// see how many loops per second can be made as an idle index
long  load=0;
long  lastLoad=millis();
long  cacheLoop=millis();




while (true) {
  // get channel data
  for (int chan=0;chan< RADIOCOUNT;chan++) {
    if ( myrx.dataready(chan) ) {
      returnstr="";
      //   appendAndParse( returnstr ,chan , myrx.getNext(chan) );
      appendAndParseFrame( frame ,chan , myrx.getNext(chan) );
      if ( frame.returnstring != "" ) {

        bool cached=0;
        //      Serial.print("CACHESEARCH");
        for (int l=0;l<CACHESIZE;l++) {
          //        Serial.print(".");
          if (cache[chan][l]==frame.crc){
            //    Serial.println("CACHE HIT ");
            cachecount[chan][l]=CACHEDELAY;
            cached=1;
            break;
          }
        }
        //Serial.println(cached);
        if (cached==0){
          //    Serial.print("CACHEADD");
          for (int l=0;l<CACHESIZE;l++) {
            //  Serial.print(".");
            if (cache[chan][l]==0){
              //Serial.print("CACHE ADD ");
              cachecount[chan][l]=CACHEDELAY;
              cache[chan][l]=frame.crc;
              if (l==CACHESIZE-1) {
                Serial.println("CACHE FULL");
              }
              break;
            }
        }

        //parse
        //     Serial.println(frame.returnstring);
        Serial.println(getReturnstring(frame));
        frame.returnstring=getReturnstring(frame);
        //Serial.print("CRC ");
        //Serial.println(frame.crc);

        long decodeStart=millis();
	      //myrx.disableReceive( chan );
	      int ret= frame_decode(frame);
	      if ( ret == 0 ) {
          //   Serial.println(returnstr);
	      }
	      returnstr="";

        // Serial.print("decode-time:");
        // long decodeTime=millis()-decodeStart;
        //Serial.println(decodeTime);
	      //myrx.enableReceive( chan );
      }
    }
  }
}

// Serial
serialEvent(); //call the function





if ( true ) {
  load++;
  long current=millis();

if (current > cacheLoop + 100 ) {
  cacheLoop=current;
           //Serial.println("CACHECLEAN");
           for (int chan=0;chan< RADIOCOUNT;chan++) {
      for (int l=0;l<CACHESIZE;l++) {
            if ( cachecount[chan][l] > 0 ) {
              if (cachecount[chan][l]>100) {
                cachecount[chan][l]-=100;
              } else {
                cachecount[chan][l]=0;
                cache[chan][l]=0;
              }
            }
          }
        }
      }
      
      // every 10 seconds show loops and memory
      if ( current > lastLoad + 10000 ) {
        lastLoad=current;
        Serial.print( F("LoadLoops:"));
        Serial.print( load);
        for (int chan=0;chan< RADIOCOUNT ;chan++) {
          Serial.print( F(" datacount"));
          Serial.print( chan );

          Serial.print( ":");
          Serial.print( myrx.dataCount(chan) );
        }
        Serial.print( F(" Freeram:"));

        Serial.println(freeRam());
        load=0;
      }
    }


  } //while
} //loop
