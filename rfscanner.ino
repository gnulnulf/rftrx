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
  serialInit();
  cmd_version("");
  cmd_showradios("");


  setupRadios();
  setupTX();

  myrx.enableReceive( 0 );
  myrx.enableReceive( 1 );
  //myrx.disableReceive( 0 );
 // myrx.disableReceive( 1 );

  cmd_txframe("");
//  myrx.enableReceive( 2);
} //setup


/*
 * LOOP
 */




void loop() {

    RFframe frame;



//  int col=0;

// om te zien hoeveel loops per seconde gemaakt worden
long  load=0;
long  lastLoad=millis();

while (true) {

// get channel data
for (int chan=0;chan< RADIOCOUNT;chan++) {
if ( myrx.dataready(chan) ) {
  returnstr="";
//   appendAndParse( returnstr ,chan , myrx.getNext(chan) );
   appendAndParseFrame( frame ,chan , myrx.getNext(chan) );
    if ( frame.returnstring != "" ) {
      Serial.println(frame.returnstring);    
	//myrx.disableReceive( chan );
	int ret= frame_decode(frame);
	if ( ret == 0 ) {
        //   Serial.println(returnstr);    
	}
	returnstr="";  
	//myrx.enableReceive( chan );
	 
    }
}
}

// Serial
serialEvent(); //call the function









if ( true ) {
  load++;
  long current=millis();
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





