#include "rftrx.h"

void setupRadios() {
#ifdef RADIO1ACTIVE

pinMode(RECEIVER1_VCC, OUTPUT);
pinMode(RECEIVER1_GAIN, OUTPUT);

digitalWrite(RECEIVER1_VCC,HIGH); //receiver1 on
pinMode(RECEIVER1_DATA, INPUT_PULLUP);
#endif

#ifdef RADIO2ACTIVE

pinMode(RECEIVER2_VCC, OUTPUT);
pinMode(RECEIVER2_GAIN, OUTPUT);
pinMode(4, OUTPUT);  //receiver2 enable
digitalWrite(RECEIVER2_VCC,HIGH); //receiver2 on
pinMode(RECEIVER2_DATA, INPUT_PULLUP);

#endif

#ifdef RADIO3ACTIVE
pinMode(RECEIVER3_DATA, INPUT_PULLUP);
#endif

//Serial.println("=TRANSn:pulse:preamble_repeats;preamble_string;data_repeats;data_string;post_repeats;post_string;#");

//rf= new RTFTRX;

pinMode(16, OUTPUT);


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

digitalWrite(TRANSMITTER1_DATA,LOW); //tx data 0
digitalWrite(TRANSMITTER1_VCC,LOW); //tx off




//pinMode(29, OUTPUT);
//    digitalWrite(26,1);/
//    digitalWrite(27,0);
//    digitalWrite(28,0);
//    digitalWrite(29,0);
//pinMode(11, OUTPUT);
//pinMode(14, OUTPUT);
//pinMode(15, OUTPUT);



//Board         int.0 int.1 int.2 int.3 int.4 int.5
//Uno, Ethernet 2 3        
//Mega2560  2 3 21  20  19  18

myrx.initInterrupt(4,0);
// //myrx.initInterrupt(5,0);
myrx.initInterrupt(0,1);
//myrx.initInterrupt(5,2);


}
