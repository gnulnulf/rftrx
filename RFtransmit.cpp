
#include "rftrx.h"

void setupTX(){
  Serial.println("Setup transmit hardware");


  
}
int sendframe(RFframe &frame,int channel, int repeat){
  int length = frame.data.length();
    Serial.print("Send FRAME on radio ");
    Serial.println( channel );
    Serial.print("PERIOD: ");
    Serial.println( frame.period );
    Serial.print("DATA: ");
    Serial.println( frame.data );

    Serial.print("count: ");
    Serial.println( frame.data.length() );
    Serial.print("repeat: ");
    Serial.println( repeat );

    bool state=0;

  long t[ length +1 ];
        state=1;
    for ( int i=0; i< (length) ;i++) {
      //Serial.print(state?"ON":"OFF");
      //Serial.print(": ");
      //Serial.println( duration );
      long duration = frameCharToInt(frame.data.charAt(i)) * frame.period ;

//  Serial.print(state);     
//  Serial.print(" - ");     
 //     Serial.println(frameCharToInt(frame.data.charAt(i)));
      if ( duration > 50) duration -= 10;
      t[ i] = duration;
        state=state?0:1;

    }
  
          digitalWrite(TRANSMITTER1_DATA,LOW);
          digitalWrite(TRANSMITTER1_VCC,HIGH);
  noInterrupts();
    for ( int j=0; j<repeat;j++) {
        state=1;
    for ( int i=0; i< (length);i++) {
          digitalWrite(TRANSMITTER1_DATA,state);
          delayMicroseconds(t[i]);
        state=state?0:1;
      }
//      Serial.print("STATE=");
  //    Serial.print(state);
 //         digitalWrite(TRANSMITTER1_DATA,HIGH);
  //        delayMicroseconds(150);
//          digitalWrite(TRANSMITTER1_DATA,LOW);
  //        delayMicroseconds(150*31);

          
    }
          digitalWrite(TRANSMITTER1_DATA,LOW);
          digitalWrite(TRANSMITTER1_VCC,LOW);
      interrupts();
    return 0;
}


