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


 
//class RFtrx;
#include <RFtrx.h>
#include <Regexp.h>


int freeRam ();
void appendAndParse(String &dest, int channel, long d);
void serialEvent();
int decode(String &frame);

#define BVERSION "20151009"

#define RECEIVER1 "433A"
#define RECEIVER1_DATA 19
#define RECEIVER1_VCC 16
#define RECEIVER1_GAIN 21

#define RECEIVER2 "868A"
#define RECEIVER2_DATA 2
#define RECEIVER2_VCC 4
#define RECEIVER2_GAIN 5


#define RECEIVER3 "IR38"
#define RECEIVER3_DATA 18
//#define RECEIVER3_VCC 5

#define TRANSMITTER1 "433A"
#define TRANSMITTER1_DATA 14
#define TRANSMITTER1_VCC 15


#define TRANSMITTER3 "IR38"
#define TRANSMITTER3_DATA 17
//#define TRANSMITTER3_VCC 

#define FRAMESIZE 250
#define MIN_FRAMESIZE 24
#define SPACELEN_TOLERANCE 5

RFtrx myrf = RFtrx();


//String receiverName[3]= {RECEIVER1,RECEIVER2,RECEIVER3}

// serial
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete


String returnstr;


int count[3]={0,0,0};
long frame[3][ FRAMESIZE ];
String receiver[3]={RECEIVER1,RECEIVER2,RECEIVER3};



void setup() {
  Serial.begin(57600);
  inputString.reserve(250);
  returnstr.reserve(200);
  //Serial.flush();
  delay(2500);
//Serial.println( BVERSION );
Serial.print("+RFTRX:VERSION:");
Serial.print( BVERSION );
Serial.println("#");
Serial.print("+");
Serial.print( RECEIVER1);
Serial.println(":NAME:RFSCANNER1#");
Serial.print("+");
Serial.print( RECEIVER2);
Serial.println(":NAME:RFSCANNER2#");
Serial.print("+");
Serial.print( RECEIVER3);
Serial.println(":NAME:RFSCANNER3#");
Serial.println("=RECV:bits:marker:period:sizeInPeriods#");
//Serial.println("=TRANSn:pulse:preamble_repeats;preamble_string;data_repeats;data_string;post_repeats;post_string;#");

//rf= new RTFTRX;

pinMode(16, OUTPUT);
pinMode(4, OUTPUT);  //receiver2 enable

/*
De Tranceiver heeft een speciale puls nodig om om te schakelen tussen zenden/ontvangen. 
Op de mega zitten vier poorten die we gebruiken om de aangesloten hardware te detecteren : 
D26, D27, D28, D29. Als je deze lijnen resp. H,L,L,L maakt, dan zal de software de benodigde puls afgeven op D22.
*/
//pinMode(22, INPUT);

      pinMode(22,OUTPUT);
      digitalWrite(22,HIGH);
pinMode(13, OUTPUT);

pinMode(TRANSMITTER1_VCC, OUTPUT);
pinMode(TRANSMITTER1_DATA, OUTPUT);

pinMode(RECEIVER1_VCC, OUTPUT);
pinMode(RECEIVER1_GAIN, OUTPUT);

pinMode(RECEIVER2_VCC, OUTPUT);
pinMode(RECEIVER2_GAIN, OUTPUT);

digitalWrite(TRANSMITTER1_DATA,LOW); //tx data 0
digitalWrite(TRANSMITTER1_VCC,LOW); //tx off

digitalWrite(RECEIVER1_VCC,HIGH); //receiver1 on
digitalWrite(RECEIVER2_VCC,HIGH); //receiver2 on



//pinMode(29, OUTPUT);
//		digitalWrite(26,1);/
//		digitalWrite(27,0);
//		digitalWrite(28,0);
//		digitalWrite(29,0);
//pinMode(11, OUTPUT);
//pinMode(14, OUTPUT);
//pinMode(15, OUTPUT);

pinMode(RECEIVER1_DATA, INPUT_PULLUP);
pinMode(RECEIVER2_DATA, INPUT_PULLUP);
pinMode(RECEIVER3_DATA, INPUT_PULLUP);

//Board	        int.0	int.1	int.2	int.3	int.4	int.5
//Uno, Ethernet	2	3	 	 	 	 
//Mega2560	2	3	21	20	19	18

myrf.initInterrupt(4,0);
//myrf.initInterrupt(5,0);
myrf.initInterrupt(0,1);
myrf.initInterrupt(5,2);


}


void loop() {
  int col=0;

// om te zien hoeveel loops per seconde gemaakt worden
long  load=0;
long  lastLoad=millis();

while (true) {

// get channel data
for (int chan=0;chan<3;chan++) {
if ( myrf.dataready(chan) ) {
  returnstr="";
   appendAndParse( returnstr ,chan , myrf.getNext(chan) );
    if ( returnstr != "" ) {
       		int ret= decode(returnstr);
	if ( ret == 0 ) {
           Serial.println(returnstr);    
}
          returnstr="";  
	 
    }
}
}

// Serial
serialEvent(); //call the function
  // print the string when a newline arrives:
  if (stringComplete) {
    Serial.print("+VERWERK:");
    Serial.println(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }





if ( true ) {
  load++;
  long current=millis();
  if ( current > lastLoad + 10000 ) {
    lastLoad=current;
    Serial.print( "LoadLoops:");
    Serial.print( load);


for (int chan=0;chan<3;chan++) {
    Serial.print( " datacount");
    Serial.print( chan );

    Serial.print( ":");
    Serial.print( myrf.dataCount(chan) );
}
    Serial.print( " Freeram:");

     Serial.println(freeRam()); 
    load=0;
  }
}


} //while
} //loop



// 
// decode string
//
// detect type of encoding
//
int decode(String &frame) {
  
  int state;
  Serial.print("DECODE:");  

  int frameLen = frame.length();
  int startOfData = frame.lastIndexOf(':') + 1; 
  int dataLen = frameLen - startOfData-1;

  // count pulselengths to find the high and low values
  int pulses[16];

	//the most common pulse is probably the spacer
	int commonPulse=0;

  for (int i = 0; i<16;i++) {
    pulses[i]=0;
  }

  for (int i = 0; i<dataLen;i++) {
    char c = frame.charAt( startOfData + i );
    if ( c > '9') {
      c-='A' +10;
    } else {
      c-='0';
  }

  if ( c < 16 ) {
    pulses[ c ]++;
  }
}



for (int i = 0; i<16;i++) {
  if ( pulses[i] > 0 ) {
	if ( pulses[i] > pulses[commonPulse]) commonPulse=i;
  Serial.print( i );
  Serial.print( "=" );
  Serial.print( pulses[i] );
  Serial.print( "," );
  }
}


Serial.print( " common=" );
Serial.println( commonPulse );


Serial.println( (dataLen - SPACELEN_TOLERANCE)/2 );


// if more then half of the pulses have the same lenght it's probably a spacer
if ( pulses[ commonPulse ] > ( (dataLen - SPACELEN_TOLERANCE)/2) ) {
Serial.print( "spacelen test odd" );

int oddCount=0;
int evenCount=0;
char commonChar;

if ( commonPulse > 9) {
	commonChar='A'+ commonPulse -10;
} else {
	commonChar='0'+ commonPulse ;
	
}
  for (int i = 0; i<dataLen;i++) {
    char c = frame.charAt( startOfData + i );
	if (c == commonChar ){
		if ( i & 1  ) {
			oddCount++;
		} else {
			evenCount++;
		}

	}
}
int oddOffset=0;
int spacelen=0;

Serial.print( oddCount );
Serial.print( " even " );

Serial.println( evenCount );

if ( evenCount > ( (dataLen - SPACELEN_TOLERANCE)/2) ) {
Serial.println( "spacelen even" );
	oddOffset=0;
	spacelen=1;
}
if ( oddCount > ( (dataLen -SPACELEN_TOLERANCE)/2) ) {
Serial.println( "spacelen odd" );
	oddOffset=1;
	spacelen=1;
}

if ( spacelen == 1 ) {
      Serial.print( "SPACELEN with offset" );    
      Serial.println( oddOffset ) ;    
	

}

/*  int oneData = frame.indexOf(String(commonPulse,HEX),startOfData);
  int notzero=0;
  if ( oneData-startOfData < 4) { 
    for(int i=oneData;i<frameLen-1;i+=2){
      if (frame.charAt( i ) != '1' ) notzero=1;
    }
    if ( notzero = 0 ) {
      Serial.println( "SPACELEN" );    
    } else {
      Serial.println( "TRISTATE?" );
    }
    
  
  
  
  }

*/
}


/*
char str[256];
if ( frame_len > 255 ) return 0;

frame.toCharArray(str, frame_len) ;

MatchState ms;
ms.Target(str);

// >433A:49:10688:260:z4141414141414311414143114311431141414311431141411#

char result = ms.Match (":", 4);
Serial.println(result);
 if (result == REGEXP_MATCHED)
 {

   char buf [256];  // large enough to hold expected string
     Serial.println("match");
          Serial.println (ms.GetMatch (buf));

   // matching offsets in ms.capture


Serial.print ("Captures: ");
Serial.println (ms.level);

for (int j = 0; j < ms.level; j++)
 {
 Serial.print ("Capture number: ");
 Serial.println (j, DEC);
 Serial.print ("Text: '");
 Serial.print (ms.GetCapture (buf, j));
 Serial.println ("'");
 
 } // end of for each capture 
 
 }

*/
  return state;
}




/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
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
Serial.println("LOW ERROR");
//      period*= 1.5;
for (int i=0;i<count[ channel ];i++) {
        if ( frame[ channel ][i]  < period * 2 ) {
        lowAvg+= frame[ channel ][i];
        lowCount++;
        }
}
// use the average of the small
period = lowAvg / lowCount;
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
		if ( count[ channel ] > FRAMESIZE ) {
			// Serial.println( "OVERFLOW! ");
			count[ channel ]=0;
		}
	}
} //einde addparse


// for debugging purposes, return the amount of free ram
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

