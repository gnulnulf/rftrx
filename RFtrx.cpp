/*
 * RFtrx library v1.0 for Arduino
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
 
#include "RFtrx.h"


unsigned int RFtrx::rxtail[3]={0,0,0};
unsigned int RFtrx::rxenable[3]={0,0,0};
unsigned int RFtrx::rxhead[3]={0,0,0};
unsigned long RFtrx::lastTime[3]={0,0,0};


//unsigned long RFtrx::minStartLength=2500;
//unsigned long RFtrx::maxStartLength=20000;
//unsigned long RFtrx::minPeriodLength=200;
//unsigned long RFtrx::maxPeriodLength=4000;

unsigned long RFtrx::minStartLength=2200;
unsigned long RFtrx::maxStartLength=20000;
unsigned long RFtrx::minPeriodLength=150;
unsigned long RFtrx::maxPeriodLength=2000;

bool RFtrx::activedata[3]={0,0,0};
bool RFtrx::glitch[3]={0,0,0};
unsigned long volatile RFtrx::rxbuffer[3][ RX_SIZE +4 ];



 //constructor
 RFtrx::RFtrx() {
//	Serial.println("RFtrx constructor");
	//long currentTime=micros();
	//lastTime=micros();
	//rxtail=0;
	//rxhead=0;
	//minStartLength=3700;
	//maxStartLength=20000;
	//setMinPeriodLength(150);
	//setMaxPeriodLength(2000);
 }
 
 void RFtrx::initInterrupt(int vector,int channel=0) {
 if ( channel == 0)     attachInterrupt(vector, receiveInterruptChannel0, CHANGE);
 if ( channel == 1)     attachInterrupt(vector, receiveInterruptChannel1, CHANGE);
 if ( channel == 2)     attachInterrupt(vector, receiveInterruptChannel2, CHANGE);

//	Serial.println("RFtrx init");
//    attachInterrupt(0, receiveInterrupt, CHANGE);
	
}



void RFtrx::enableReceive(int channel=0) {
	rxenable[ channel] =1;
	//Serial.print("+");
}

void RFtrx::disableReceive(int channel=0) {
	//Serial.print("-");
	rxenable[ channel] =0;
}


bool RFtrx::dataready(int channel=0)  {
		return (( rxhead[channel] == rxtail[channel] ) ?false:true );
}


int RFtrx::dataCount(int channel=0) {
	if ( rxhead[ channel] == rxtail[ channel]) return 0;
	if ( rxhead[ channel] > rxtail[ channel] ) {
			return rxhead[ channel]-rxtail[ channel];
	} else {
		return RX_SIZE+rxhead[ channel]-rxtail[ channel];
	}
}




	
void RFtrx::setMinStartLength( unsigned long length ) {minStartLength = length;}
void RFtrx::setMaxStartLength( unsigned long length ) {maxStartLength = length;}
	
void RFtrx::setMinPeriodLength( unsigned long length ) {minPeriodLength = length;}
void RFtrx::setMaxPeriodLength( unsigned long length ) {maxPeriodLength = length;}
	
long RFtrx::getNext(int channel=0)  {
	long r=-1;
	if ( rxhead[ channel] == rxtail[ channel] ) {
		Serial.println("ERROR NO DATA READY");
	} else {
		r=rxbuffer[ channel ][ rxtail[ channel] ];
		rxtail[ channel]++;
		if ( rxtail[ channel] > RX_SIZE ) rxtail[ channel]=0;
	}
	return r;
}	

void RFtrx::receiveInterruptChannel0() {
	if ( rxenable[0] == 1 ) RFtrx::receiveInterrupt(0);
}

void RFtrx::receiveInterruptChannel1() {
	 if ( rxenable[1] == 1 ) RFtrx::receiveInterrupt(1);
}

void RFtrx::receiveInterruptChannel2() {
	 if ( rxenable[2] == 1 ) RFtrx::receiveInterrupt(2);
}

// receive a frame flank for channel n
void RFtrx::receiveInterrupt(int channel=0) {
	long	currentTime=micros();
	
	// clock wrap would not happen much
	if ( currentTime < RFtrx::lastTime[ channel] ) {
		// clock wrap
		RFtrx::lastTime[ channel]=currentTime;
		RFtrx::activedata[ channel]=0;
		return;
	}

	// the duration of the pulse or space
	word duration=currentTime - RFtrx::lastTime[ channel];
	
	//if the pulse is too short it's probably a glitch
	if (duration < RFtrx::minPeriodLength) {
		// no spikes
		glitch[ channel]=1;
		return;
	}
	
	// glitch stops at first normal length
	if ( glitch[ channel] == 1 ) {
		//end of spike.
		glitch[ channel]=0;
		return;
	}

	// if frame is active but pulse is longer then the max period it's a stop 
	if ((activedata[ channel] == 1 )&& ( duration >RFtrx::maxPeriodLength)) {
		RFtrx::activedata[ channel]=0;
		// do we need to know it was a large pulse?
		RFtrx::rxbuffer[ channel][RFtrx::rxhead[ channel]]= 0;
		RFtrx::rxhead[ channel]++;
		if ( RFtrx::rxhead[ channel] > RX_SIZE ) RFtrx::rxhead[ channel]=0;
		if ( RFtrx::rxhead[ channel] == RFtrx::rxtail[ channel] ) {
			//overflow
		}
		return;
	}
	
	// longer then max start length means no frame at all
	if (duration >RFtrx::maxStartLength) {
		RFtrx::activedata[ channel]=0;
		return;
	}
	
	// start of frame
	if ( (duration > RFtrx::minStartLength) && (duration < RFtrx::maxStartLength) && RFtrx::activedata[ channel] == 0 ){
		RFtrx::activedata[ channel]=1;
		RFtrx::rxbuffer[ channel][RFtrx::rxhead[ channel]]= 0;
		RFtrx::rxhead[ channel]++;
		if ( RFtrx::rxhead[ channel] > RX_SIZE ) RFtrx::rxhead[ channel]=0;
		if ( RFtrx::rxhead[ channel] == RFtrx::rxtail[ channel] ) {
			//overflow
		}
	}
	
	//if (duration > RFtrx::maxStartLength) {
	//	return;
	//}

	RFtrx::rxbuffer[ channel][RFtrx::rxhead[ channel]]= duration;
	RFtrx::rxhead[ channel]++;
	if ( RFtrx::rxhead[ channel] > RX_SIZE ) RFtrx::rxhead[ channel]=0;
	if ( RFtrx::rxhead[ channel] == RFtrx::rxtail[ channel] ) {
		//overflow
	}
	//Serial.println(duration);
	//digitalWrite(13,digitalRead(2));
	RFtrx::lastTime[ channel]=currentTime;
}

void RFtrx::receiveInterruptA() {
	long	currentTime=micros();
	if ( currentTime < RFtrx::lastTime[0] ) {
		// clock wrap
		RFtrx::lastTime[0]=currentTime;
		RFtrx::activedata[0]=0;
		return;
	}

	word duration=currentTime - RFtrx::lastTime[0];
	if (duration < RFtrx::minPeriodLength) {
		// no spikes
		glitch[0]=1;
		return;
	}
	if ( glitch[0] == 1 ) {
		//end of spike.
		glitch[0]=0;
		return;
	}

	if ((activedata[0] == 1 )&& ( duration >RFtrx::maxPeriodLength)) {
	
				RFtrx::activedata[0]=0;
	RFtrx::rxbuffer[0][RFtrx::rxhead[0]]= 0;
	RFtrx::rxhead[0]++;
	if ( RFtrx::rxhead[0] > RX_SIZE ) RFtrx::rxhead[0]=0;
	if ( RFtrx::rxhead[0] == RFtrx::rxtail[0] ) {
		//overflow

	}

//		digitalWrite(13,0);
		return;
	}

	
	if (duration >RFtrx::maxStartLength) {
				RFtrx::activedata[0]=0;

//		digitalWrite(13,0);
		return;
	}
	
	if ( (duration > RFtrx::minStartLength) && (duration < RFtrx::maxStartLength) && RFtrx::activedata[0] == 0 ){
			RFtrx::activedata[0]=1;
	RFtrx::rxbuffer[0][RFtrx::rxhead[0]]= 0;
	RFtrx::rxhead[0]++;
	if ( RFtrx::rxhead[0] > RX_SIZE ) RFtrx::rxhead[0]=0;
	if ( RFtrx::rxhead[0] == RFtrx::rxtail[0] ) {
		//overflow

	}
			
	}
	
	if (duration > RFtrx::maxStartLength) {

			

//		digitalWrite(13,0);
		return;
	}


	RFtrx::rxbuffer[0][RFtrx::rxhead[0]]= duration;
	RFtrx::rxhead[0]++;
	if ( RFtrx::rxhead[0] > RX_SIZE ) RFtrx::rxhead[0]=0;
	if ( RFtrx::rxhead[0] == RFtrx::rxtail[0] ) {
		//overflow

	}
	//Serial.println(duration);
	//digitalWrite(13,digitalRead(2));
	RFtrx::lastTime[0]=currentTime;

}

