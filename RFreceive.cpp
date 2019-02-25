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
 @date 20180408 start of cleanup and migrate to different glitch algoritm
 */
 
//#include "RFreceive.h"
#include "rftrx.h"

#define NEWGLITCH

//create the class
RFreceive myrx = RFreceive();

// depending on the radiocount the receiver and count arrays are configured
long frame[ RADIOCOUNT ][ FRAMESIZE ];
#if RADIOCOUNT==3
String receiver[3]={RECEIVER1,RECEIVER2,RECEIVER3};
int count[ 3 ]={0,0,0};
#endif
#if RADIOCOUNT==2
String receiver[2]={RECEIVER1,RECEIVER2};
int count[ 2 ]={0,0};
#endif
#if RADIOCOUNT==1
String receiver[1]={RECEIVER1};
int count[ 1 ]={0};
#endif

unsigned int RFreceive::rxtail[ RADIOCOUNT ];
unsigned int RFreceive::rxenable[ RADIOCOUNT ];
unsigned int RFreceive::rxhead[ RADIOCOUNT ];
unsigned long RFreceive::lastTime[ RADIOCOUNT ];

//unsigned long RFreceive::minStartLength=2500;
//unsigned long RFreceive::maxStartLength=20000;
//unsigned long RFreceive::minPeriodLength=200;
//unsigned long RFreceive::maxPeriodLength=4000;

unsigned long RFreceive::minStartLength=1000;
unsigned long RFreceive::maxStartLength=20000;
unsigned long RFreceive::minPeriodLength=400;
unsigned long RFreceive::maxPeriodLength=3600;

bool RFreceive::pin[ RADIOCOUNT ];
bool RFreceive::activedata[ RADIOCOUNT ];
bool RFreceive::glitch[ RADIOCOUNT ];

signed long RFreceive::filter[ RADIOCOUNT ];
unsigned long volatile RFreceive::rxbuffer[ RADIOCOUNT ][ RX_SIZE +4 ];

//constructor
RFreceive::RFreceive() {
  //  Serial.println("RFreceive constructor");
  //long currentTime=micros();
  //lastTime=micros();
  //rxtail=0;
  //rxhead=0;
  //minStartLength=3700;
  //maxStartLength=20000;
  //setMinPeriodLength(150);
  //setMaxPeriodLength(2000);

  //reset frame arrays  
  for ( int i=0; i<RADIOCOUNT ; i++) {
    RFreceive::activedata[ i ]=0;
    RFreceive::glitch[ i ]=0;
    RFreceive::rxtail[ i ]=0;
    RFreceive::rxenable[ i ]=0;
    RFreceive::rxhead[ i ]=0;
    RFreceive::lastTime[ i ]=0;
  } 
}  //end constructor

// setters
void RFreceive::setMinStartLength( unsigned long length ) {minStartLength = length;}
void RFreceive::setMaxStartLength( unsigned long length ) {maxStartLength = length;}
  
void RFreceive::setMinPeriodLength( unsigned long length ) {minPeriodLength = length;}
void RFreceive::setMaxPeriodLength( unsigned long length ) {maxPeriodLength = length;}

//! attach the vector dependend receiver code to the vector
// I don't know if this can be done different.
//! @param vector arduino interrupt number  
//! @param channel Channel number (0-3)
void RFreceive::initInterrupt(int vector,int channel=0) {
  if ( channel == 0)     attachInterrupt(vector, receiveInterruptChannel0, CHANGE);
  if ( channel == 1)     attachInterrupt(vector, receiveInterruptChannel1, CHANGE);
  if ( channel == 2)     attachInterrupt(vector, receiveInterruptChannel2, CHANGE);
  //  Serial.println("RFreceive init");
  //    attachInterrupt(0, receiveInterrupt, CHANGE);
} // end RFreceive::initInterrupt

//! enable receiving for channel
//! @param channel Channel number (0-3)
void RFreceive::enableReceive(int channel=0) {
  rxenable[ channel] =1;
} // end RFreceive::enableReceive

//! disable receiving for channel
//! @param channel Channel number (0-3)
void RFreceive::disableReceive(int channel=0) {
  rxenable[ channel] =0;
} // end RFreceive::disableReceive

//! Is data ready for channel
//! @param channel Channel number (0-3)
//! @return bool data ready
bool RFreceive::dataready(int channel=0)  {
    return (( rxhead[channel] == rxtail[channel] ) ?false:true );
} // end RFreceive::dataready

//! get amount of data items ready
//! @param channel Channel number (0-3)
//! @return int count of data ready
int RFreceive::dataCount(int channel=0) {
  if ( rxhead[ channel] == rxtail[ channel]) return 0;
  if ( rxhead[ channel] > rxtail[ channel] ) {
      return rxhead[ channel]-rxtail[ channel];
  } else {
    return RX_SIZE+rxhead[ channel]-rxtail[ channel];
  }
} // end RFreceive::dataCount


//! get string of ascii representation for the RFframe 
//! @param &f refrence to RF frame
//! @return String of ascii frame data
String getReturnstring( RFframe &f ){
  String r=">"+f.receiver+":"+f.count+":"+f.startperiod+":"+f.period+":"+f.reldata+"#";
  return r;
} // end getReturnstring
  
//! get next data item
//! @param channel Channel number (0-3)
//! @return long data value
long RFreceive::getNext(int channel=0)  {
  long r=-1;
  if ( rxhead[ channel] == rxtail[ channel] ) {
    Serial.println(F("ERROR NO DATA READY"));
  } else {
    r=rxbuffer[ channel ][ rxtail[ channel] ];
    rxtail[ channel]++;
    if ( rxtail[ channel] >= RX_SIZE ) rxtail[ channel]=0;
  }
  return r;
} //end RFreceive::getNext

//! Interrupt Routine specific for radio 0 
void RFreceive::receiveInterruptChannel0() {
  if ( rxenable[0] == 1 ) {
	RFreceive::receiveInterrupt(0);
	RFreceive::pin[0] = ( PORTD & 0x00000100) ;
	
	}
} // end RFreceive::receiveInterruptChannel0 

//! Interrupt Routine specific for radio 1
void RFreceive::receiveInterruptChannel1() {
   if ( rxenable[1] == 1 ) {
	RFreceive::receiveInterrupt(1);
	RFreceive::pin[1] = ( PORTE & 0x00010000) ;
	}
} // end RFreceive::receiveInterruptChannel1

//! Interrupt Routine specific for radio 2 
void RFreceive::receiveInterruptChannel2() {
   if ( rxenable[2] == 1 ) {
	RFreceive::receiveInterrupt(2);
	RFreceive::pin[2] = ( PORTJ & 0x00000010) ;
	}
} // end RFreceive::receiveInterruptChannel2

//! receive a frame flank for channel n
//! This is the shared part of the interrupt routines 
//! @param channel Channel number (0-3)
void RFreceive::receiveInterrupt(int channel=0) {
  long  currentTime=micros();
  
  // clock wrap would not happen much
  if ( currentTime < RFreceive::lastTime[ channel] ) {
    // clock wrap
    RFreceive::lastTime[ channel]=currentTime;
    RFreceive::activedata[ channel]=0;
    return;
  }

  // the duration of the pulse or space
  word duration=currentTime - RFreceive::lastTime[ channel];
  
#ifdef NEWGLITCH
	if (RFreceive::pin[ channel ]) {
		RFreceive::filter[ channel ] += duration;
	} else {
		RFreceive::filter[ channel ] -= duration;
	}
  if ((RFreceive::filter[ channel ] < RFreceive::minPeriodLength) && (RFreceive::filter[ channel ] > 0)) {
    // no spikes
    glitch[ channel]=1;
    return;
  }
  
        if (RFreceive::pin[ channel ]) {
                duration = RFreceive::filter[ channel ];
		RFreceive::filter[ channel ] = RFreceive::minPeriodLength;
        } else {
                duration = - RFreceive::filter[ channel ] + RFreceive::minPeriodLength ;
		RFreceive::filter[ channel ] = 0;
        }

#endif
#ifndef NEWGLITCH
  //if the pulse is too short it's probably a glitch
  if (duration < RFreceive::minPeriodLength) {
    // no spikes
    glitch[ channel]=1;
    return;
}

#endif
  // glitch stops at first normal length
  if ( glitch[ channel] == 1 ) {
    //end of spike.
    glitch[ channel]=0;
    return;
  }

  // if frame is active but pulse is longer then the max period it's a stop 
  if ((activedata[ channel] == 1 )&& ( duration >RFreceive::maxPeriodLength)) {
    RFreceive::activedata[ channel]=0;
    // do we need to know it was a large pulse?
    RFreceive::rxbuffer[ channel][RFreceive::rxhead[ channel]]= 0;
    RFreceive::rxhead[ channel]++;
    if ( RFreceive::rxhead[ channel] >= RX_SIZE ) RFreceive::rxhead[ channel]=0;
    if ( RFreceive::rxhead[ channel] == RFreceive::rxtail[ channel] ) {
      //overflow
    }
    return;
  }
  
  // longer then max start length means no frame at all
  if (duration >RFreceive::maxStartLength) {
    RFreceive::activedata[ channel]=0;
    return;
  }
  
  // start of frame
  if ( (duration > RFreceive::minStartLength) && (duration < RFreceive::maxStartLength) && RFreceive::activedata[ channel] == 0 ){
    RFreceive::activedata[ channel]=1;
    RFreceive::rxbuffer[ channel][RFreceive::rxhead[ channel]]= 0;
    RFreceive::rxhead[ channel]++;
    if ( RFreceive::rxhead[ channel] >= RX_SIZE ) RFreceive::rxhead[ channel]=0;
    if ( RFreceive::rxhead[ channel] == RFreceive::rxtail[ channel] ) {
      //overflow
    }
  }
  
  //if (duration > RFreceive::maxStartLength) {
  //  return;
  //}

  RFreceive::rxbuffer[ channel][RFreceive::rxhead[ channel]]= duration;
  RFreceive::rxhead[ channel]++;
  if ( RFreceive::rxhead[ channel] >= RX_SIZE ) RFreceive::rxhead[ channel]=0;
  if ( RFreceive::rxhead[ channel] == RFreceive::rxtail[ channel] ) {
    //overflow
  }
  //Serial.println(duration);
  //digitalWrite(13,digitalRead(2));
  RFreceive::lastTime[ channel]=currentTime;
} // end RFreceive::receiveInterrupt 


void RFreceive::receiveInterruptA() {
  long  currentTime=micros();
  if ( currentTime < RFreceive::lastTime[0] ) {
    // clock wrap
    RFreceive::lastTime[0]=currentTime;
    RFreceive::activedata[0]=0;
    return;
  }

  word duration=currentTime - RFreceive::lastTime[0];
  if (duration < RFreceive::minPeriodLength) {
    // no spikes
    glitch[0]=1;
    return;
  }
  if ( glitch[0] == 1 ) {
    //end of spike.
    glitch[0]=0;
    return;
  }

  if ((activedata[0] == 1 )&& ( duration >RFreceive::maxPeriodLength)) {
  
        RFreceive::activedata[0]=0;
  RFreceive::rxbuffer[0][RFreceive::rxhead[0]]= 0;
  RFreceive::rxhead[0]++;
  if ( RFreceive::rxhead[0] >= RX_SIZE ) RFreceive::rxhead[0]=0;
  if ( RFreceive::rxhead[0] == RFreceive::rxtail[0] ) {
    //overflow

  }

//    digitalWrite(13,0);
    return;
  }

  
  if (duration >RFreceive::maxStartLength) {
        RFreceive::activedata[0]=0;

//    digitalWrite(13,0);
    return;
  }
  
  if ( (duration > RFreceive::minStartLength) && (duration < RFreceive::maxStartLength) && RFreceive::activedata[0] == 0 ){
      RFreceive::activedata[0]=1;
  RFreceive::rxbuffer[0][RFreceive::rxhead[0]]= 0;
  RFreceive::rxhead[0]++;
  if ( RFreceive::rxhead[0] >= RX_SIZE ) RFreceive::rxhead[0]=0;
  if ( RFreceive::rxhead[0] == RFreceive::rxtail[0] ) {
    //overflow

  }
      
  }
  
  if (duration > RFreceive::maxStartLength) {

      

//    digitalWrite(13,0);
    return;
  }


  RFreceive::rxbuffer[0][RFreceive::rxhead[0]]= duration;
  RFreceive::rxhead[0]++;
  if ( RFreceive::rxhead[0] >= RX_SIZE ) RFreceive::rxhead[0]=0;
  if ( RFreceive::rxhead[0] == RFreceive::rxtail[0] ) {
    //overflow

  }
  //Serial.println(duration);
  //digitalWrite(13,digitalRead(2));
  RFreceive::lastTime[0]=currentTime;

}



void appendAndParse(String &dest, int channel, long d) {
        dest="";
    if ( d == 0 ) {
    if (( count[ channel ] > MIN_FRAMESIZE ) ) {
      long period=frame[ channel ][0];
      long high=frame[ channel ][0];
      long avg=frame[ channel ][0];
      for (int i=0;i<count[ channel ];i++) {
        avg += frame[ channel ][i];
        if ( frame[ channel ][i] < period ) {
          period=frame[ channel ][i];
        }
        if ( frame[ channel ][i] > period ) {
          high=frame[ channel ][i];
        }
      }      
      avg /= count[ channel ];


// indien er minder dan 10% 1-periods zijn zal dat wel fout zijn
int countLow=0;
for (int i=0;i<count[ channel ];i++) {
        if ((( frame[ channel ][i] + period * 0.5 ) / period ) ==1 ) countLow++;
}
if (countLow< (count[channel] / 10)) {
long lowAvg=0;
int lowCount=0;
//Serial.println("LOW ERROR");
//      period*= 1.5;
for (int i=0;i<count[ channel ];i++) {
        if ( frame[ channel ][i]  < period * 2 ) {
        lowAvg+= frame[ channel ][i];
        lowCount++;
        }
}
// use the average of the small
// half periods...
//period = (lowAvg / lowCount)/2;
period = (lowAvg / lowCount);
}



                        dest=">"+receiver[ channel ]+":"+count[ channel ]+":"+frame[ channel ][0]+":"+period+":";
                        

      for (int i=0;i<=count[ channel ];i++) {
        //int p = frame[ channel ][i] / ( period *0.95 );
        int p = (frame[ channel ][i] + period*0.7) / ( period  );
        //int p = frame[ channel ][i] / ( period -5 );

//Serial.print(i);
//Serial.print(":");
//Serial.print( frame[ channel ][i]);
//Serial.print(":");
//Serial.println(p);
                                // moet 0-9a-z worden om de 31x op te vangen.
                                if ( p > 35 ) p=35;
                                if (p < 10) {
                                 dest+= p;
                                }else{
                                 char dum='a'+p-10;
                                  dest += dum;                                  
                                }
          /*
p = frame[channel][i] / ( period - 5 );
          if ( p > 15 ) p=0xf;
          dest += String( p  , HEX);
*/
      }
//Serial.println("--------");
      dest +="#";      
    }
    count[ channel ]=0;
    //Serial.println("."); 
  } else {
    frame[ channel ][count[ channel ]] = d;
    count[ channel ]++;
    if ( count[ channel ] >= FRAMESIZE ) {
       Serial.println( "OVERFLOW! ");
      count[ channel ]=0;
    }
  }
} //einde addparse


void appendAndParseFrame(RFframe &dest, int channel, long d)/*NOPROTO*/ {
  dest.returnstring="";
  if ( d == 0 ) {
    if (( count[ channel ] > MIN_FRAMESIZE ) ) {
      long period=frame[ channel ][0];
      long high=frame[ channel ][0];
      long avg=frame[ channel ][0];
      for (int i=0;i<count[ channel ];i++) {
        avg += frame[ channel ][i];
        if ( frame[ channel ][i] < period ) {
          period=frame[ channel ][i];
        }
        if ( frame[ channel ][i] > period ) {
          high=frame[ channel ][i];
        }
      }      
      avg /= count[ channel ];


  // indien er minder dan 10% 1-periods zijn zal dat wel fout zijn
  int countLow=0;
  for (int i=0;i<count[ channel ];i++) {
    if ((( frame[ channel ][i] + period * 0.5 ) / period ) ==1 ) countLow++;
  }
  if (countLow< (count[channel] / 10)) {
  long lowAvg=0;
  int lowCount=0;
  //Serial.println("LOW ERROR");
  //      period*= 1.5;
  for (int i=0;i<count[ channel ];i++) {
        if ( frame[ channel ][i]  < period * 2 ) {
        lowAvg+= frame[ channel ][i];
        lowCount++;
        }
  }
  // use the average of the small
  // half periods...
  //period = (lowAvg / lowCount)/2;
  period = (lowAvg / lowCount);
}


                        dest.returnstring=">"+receiver[ channel ]+":"+count[ channel ]+":"+frame[ channel ][0]+":"+period+":";
                        dest.reldata="";
                        dest.count=count[ channel ];
                        dest.period=period;
                        dest.startperiod=frame[ channel ][0];
                        dest.receiver=receiver[ channel ];
                        dest.crc=dest.count<<8;

    

      for (int i=0;i<=count[ channel ];i++) {
        //int p = frame[ channel ][i] / ( period *0.95 );
        int p = (frame[ channel ][i] + period*0.7) / ( period  );
        //int p = frame[ channel ][i] / ( period -5 );

//Serial.print(i);
//Serial.print(":");
//Serial.print( frame[ channel ][i]);
//Serial.print(":");
//Serial.println(p);

//dest.returnstring += frameIntToChar( p) ;

                                // moet 0-9a-z worden om de 31x op te vangen.
                                dest.crc ^=p;
                                if ( p > 35 ) p=35;
                                if (p < 10) {
                                 dest.reldata+= p;
                                }else{
                                 char dum='a'+p-10;
                                  dest.reldata += dum;                                  
                                }
          /*
p = frame[channel][i] / ( period - 5 );
          if ( p > 15 ) p=0xf;
          dest += String( p  , HEX);
*/
      }
//Serial.println("--------");
      dest.returnstring +="#";      
    }
    count[ channel ]=0;
    //Serial.println("."); 
  } else {
    frame[ channel ][count[ channel ]] = d;
    count[ channel ]++;
    if ( count[ channel ] >= FRAMESIZE ) {
       Serial.println( "OVERFLOW! ");
      count[ channel ]=0;
    }
  }
} //end addparseframe











