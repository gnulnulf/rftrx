

#include "frame_decode.h"
#include "decode_spacelen.h"

String spacelenFrame="";
String spacelenFrameInv="";

// 
// decode string
//
// detect type of encoding
//
int frame_decode(String &frame) {
  
  //int state;
int decoded=0;

  int frameLen = frame.length();
  int startOfData = frame.lastIndexOf(':') + 1; 
  int dataLen = frameLen - startOfData-1;

if ( dataLen < MIN_FRAMESIZE ) return 99;


  Serial.println(F("DECODE:######################################"));  

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


// get pulse statistics
for (int i = 0; i<16;i++) {
  if ( pulses[i] > 0 ) {
  if ( pulses[i] > pulses[commonPulse]) commonPulse=i;
//  Serial.print( i );
 // Serial.print( "=" );
 // Serial.print( pulses[i] );
 // Serial.print( "," );
  }
}

// sort the pulses 
int pulsesort[16];
for (int i = 0; i<16;i++) {
  pulsesort[i]=i;
}

for (int i = 0; i<16;i++) {
for (int j = i; j<15;j++) {
  if ( pulses[pulsesort[j]] < pulses[pulsesort[j+1]] ) {
    int dum=pulsesort[j];
    pulsesort[j]= pulsesort[j+1]; 
    pulsesort[j+1]=dum;
  }
  }
}

//Serial.print( " common=" );
//Serial.println( commonPulse );

// needed pulses
//Serial.println( (dataLen - SPACELEN_TOLERANCE)/2 );

// check for dual pulselengths, probably 1331 coded 
// ##FIXME need the real name for 1331 encoding ;-)
if ( pulses[ pulsesort[0] ]  > ( (dataLen/2 - 4)) ) {
if ( pulses[ pulsesort[0] ] + pulses[ pulsesort[1] ] > ( (dataLen - 4)) ) {
  Serial.print( "Maybe 1331" );
  
int lowpulse = pulsesort[0];
int highpulse = pulsesort[1];
if (lowpulse > highpulse ){
lowpulse = pulsesort[1];
highpulse = pulsesort[2];
}

char lowChar;
char highChar;
if ( lowpulse > 9) {
        lowChar='A'+ lowpulse -10;
} else {
        lowChar='0'+ lowpulse ;
}
if ( highpulse > 9) {
        highChar='A'+ highpulse -10;
} else {
        highChar='0'+ highpulse ;
}
String lh="";
lh+=lowChar;
lh+=highChar;
lh+=highChar;
lh+=lowChar;

String hl="";
hl+=highChar;
hl+=lowChar;
hl+=lowChar;
hl+=highChar;

int offset;
if ( frame.lastIndexOf( hl ) > 0 ) {
offset = ( frame.lastIndexOf( hl ) - startOfData ) % 1;
} else {
offset = ( frame.lastIndexOf( lh ) - startOfData ) % 1;
}

Serial.print( F(" low=") );
Serial.print( lowpulse );
Serial.print( F(" high=" ));
Serial.print( highpulse );
Serial.print( F(" lhhl=") );
Serial.print( lh );
Serial.print( F(" hllh=") );
Serial.print( hl );
Serial.print( F(" indexlhhl=" ));
Serial.print( frame.lastIndexOf( lh ) );
Serial.print( F(" indexhllh=") );
Serial.print( frame.lastIndexOf( hl ) );

Serial.print( F(" offset=") );
Serial.print( offset );
 
Serial.println( );

String data1331="";

for(int j=startOfData + offset ;j<frameLen;j+=2) {
        if (( frame.charAt( j )  == lowChar ) or ( frame.charAt( j+1 ) == lowChar )) {
    if (( frame.charAt( j )  == lowChar ) and ( frame.charAt( j + 1 )  >lowChar )) data1331+='0';
    if (( frame.charAt( j )  > lowChar ) and ( frame.charAt( j + 1 )  == lowChar )) data1331+='1';

  }
}
Serial.print( F(">DATA1331:") );
Serial.print( data1331.length() );
Serial.print( ":" );
Serial.print( data1331 );
Serial.println( );







  



}
} // end_1331

// if more then half of the pulses have the same lenght it's probably a spacer
if ( pulses[ commonPulse ] > ( (dataLen - SPACELEN_TOLERANCE)/2) ) {
//Serial.print( "spacelen test odd" );

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


//Serial.print( oddCount );
//Serial.print( " even " );

//Serial.println( evenCount );

if ( evenCount > ( (dataLen - SPACELEN_TOLERANCE)/2) ) {
//Serial.println( "spacelen even" );
  oddOffset=0;
  spacelen=1;
}
if ( oddCount > ( (dataLen -SPACELEN_TOLERANCE)/2) ) {
//Serial.println( "spacelen odd" );
  oddOffset=1;
  spacelen=1;
}

if ( spacelen == 1 ) {
   //   Serial.print( "SPACELEN with offset" );    
    //  Serial.println( oddOffset ) ;    


// temporary create strings to 
String dumframe="";
String dummatch="";
for(int j=0;j<MIN_FRAMESIZE /2 ;j++) {
  dummatch += commonChar;
}
 //     Serial.print( "SPACELEN dummatch" );    
  //    Serial.println( dummatch ) ;    
for(int j=startOfData + oddOffset ;j<dataLen;j+=2) {
  dumframe += frame.charAt( j);
}
//      Serial.print( "SPACELEN dumframe" );    
//      Serial.println( dumframe ) ;    

int markerstart = dumframe.indexOf(dummatch) * 2;
//      Serial.print( "SPACELEN markerstart" );    
//      Serial.println( markerstart ) ;    

if ( markerstart >=0 ) {
spacelenFrame="";
spacelenFrameInv="";


char splitChar = commonChar;
if ( pulses[commonPulse] < (dataLen /2) + 2) {
int splitPulse = ( pulsesort[1] + pulsesort[2] ) /2;
//      Serial.print( "SPACELEN 1214 pulse " );    
  
 //     Serial.print( splitPulse );    

  //    Serial.print( " sort1 " );    
   //   Serial.print( pulsesort[1] );    
    //  Serial.print( " sort2 " );    
     // Serial.print( pulsesort[2] );    
if ( splitPulse > 9) {
        splitChar='A'+ splitPulse -10;
} else {
        splitChar='0'+ splitPulse ;

}
      //Serial.print( " char " );    

      //Serial.println( splitChar );    



}


for(int j=startOfData + oddOffset + markerstart ;j<frameLen;j+=2) {
  if ( frame.charAt( j )  == commonChar ) { 
    char c =  frame.charAt( j -1 ) ;
    // ##FIXME
    // will go wrong if both 0 and 1 are larger then the marker
    if ( c > splitChar) {
      spacelenFrame += '1';
      spacelenFrameInv += '0';
    } else {
      spacelenFrame += '0';
      spacelenFrameInv += '1';
    }
        
  } else {
    break;
  }
  
}
  if (  decode_spacelen( spacelenFrame ) ) {
    decoded=1;
} else {
      Serial.print( F( ">SPACELENFRAME:") );    
      Serial.print( spacelenFrame.length()) ;    
      Serial.print( ":" );    
      Serial.println( spacelenFrame ) ;    
  

//      Serial.print( ">SPACELENFRAMEINV:" );    
 //     Serial.print( spacelenFrameInv.length()) ;    
  //    Serial.print( ":" );    
   //   Serial.println( spacelenFrameInv ) ;    
}




}
} //end_spacelen==1
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
  return decoded;
}


