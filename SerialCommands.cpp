
//#include "SerialCommands.h"
//#include "tools.h"
#include "rftrx.h"

String inputString;         // a string to hold incoming data
boolean stringComplete;  // whether the string is complete


Command commands[] ={
  {"help",&help,0,"Show help commands"},
  {"version",&cmd_version,0,"Show Version"},
  {"showradios",&cmd_showradios,0,"Show configured radios"},

  {"rxenable",&cmd_rxenable,0,"Enable channel n"},
  {"rxdisable",&cmd_rxdisable,0,"Disable channel n"},
  {"txframe",&cmd_txframe,0,"FRAME SEND TEST"},
  
  
  {"ping",&ping,0,"Keep alive test"},
};





// serial
void serialInit() {
    Serial.begin(57600);
    Serial.flush();


  inputString = "";         // a string to hold incoming data 
  stringComplete = false;  // whether the string is complete
} //serialInit










void parseCommand(String cmd, String param) {
  int found = 0;
  cmd.toLowerCase();
  for(int i=0;i<(sizeof(commands)/sizeof(Command)) ;i++){
    if ( commands[i].name == cmd ){
      commands[i].func( param );
      found = 1 ;
    }
  }

    if ( found == 0 ){
      Serial.println("Command not found.");
    }

} //parseCommand


/*
void loop() {
 
//parseCommand("Help","param1");
//parseCommand("version","param1");
//parseCommand("ping","param1");
//parseCommand("pinG","param1");
//parseCommand("piNg","param1");

if (stringComplete) {
    Serial.print("INPUTSTRING:");
    Serial.println(inputString);

    String param = "";
    String cmd = "";
    // command with parameters
    if ( inputString.indexOf(" ") >0 ) {
      Serial.println("MULTICOMMAND");
      cmd = inputString.substring(0, inputString.indexOf(" "));
      param = inputString.substring( inputString.indexOf(" ")+1);
  } else {
    cmd = inputString;
    
  }

Serial.print("SPLIT#");
Serial.print(cmd);
Serial.print("#");
Serial.print(param);
Serial.println("#");
    
    parseCommand(cmd,param);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }




//Command v ={"name",&test1,"help"};
//v.func("test");



delay(100);
}
*/


/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
//  while (Serial.available()) {
  if (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
       if (inChar == '\n') {
      stringComplete = true;
    } else {
    inputString += inChar;
    }
  }
if (stringComplete) {
    Serial.print("INPUTSTRING:");
    Serial.println(inputString);

    String param = "";
    String cmd = "";
    // command with parameters
    if ( inputString.indexOf(" ") >0 ) {
      Serial.println("MULTICOMMAND");
      cmd = inputString.substring(0, inputString.indexOf(" "));
      param = inputString.substring( inputString.indexOf(" ")+1);
  } else {
    cmd = inputString;
    
  }

/*
Serial.print("SPLIT#");
Serial.print(cmd);
Serial.print("#");
Serial.print(param);
Serial.println("#");
  */  
    parseCommand(cmd,param);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }


  
}





void help(String param ){
Serial.println();
Serial.println("Known commands");
Serial.println("==============");
for(int i=0;i<(sizeof(commands)/sizeof(Command)) ;i++){
  Serial.print( commands[i].name );
  Serial.print( " - ");
  Serial.println( commands[i].help );
}
Serial.println();
}

void cmd_version(String param ){

Serial.print(F("+RFTRX:VERSION:"));
Serial.print( BVERSION );
Serial.println("#");
//Serial.print("+");


  
}



void ping(String param ){
  Serial.println("pong");
}


void cmd_showradios(String param ){
  Serial.println("Show radios");

//#ifdef RADIO1ACTIVE
Serial.print( RECEIVER1);
Serial.println(F(":NAME:RFSCANNER1#"));
Serial.print("+");
//#endif

#ifdef RADIO2ACTIVE
Serial.print( RECEIVER2);
Serial.println(F(":NAME:RFSCANNER2#"));
Serial.print("+");


#endif

#ifdef RADIO3ACTIVE
Serial.print( RECEIVER3);
Serial.println(F(":NAME:RFSCANNER3#"));
Serial.println(F("=RECV:bits:marker:period:sizeInPeriods#"));
#endif

  
  //Serial.println("Date bla");

//Serial.print(F("+RFTRX:VERSION:"));
//Serial.print( BVERSION );
//Serial.println("#");
//Serial.print("+");


  
}




void test2(String param[],int paramCount){
  Serial.println("test1");
}

void cmd_rxenable(String param ){
  if ( param == "0" ) myrx.enableReceive( 0 );
  if ( param == "1" ) myrx.enableReceive( 1 );
  if ( param == "2" ) myrx.enableReceive( 2 );
  if ( param == "3" ) myrx.enableReceive( 3 );
  if ( param == "all" ) {
      for (int i=0;i<RADIOCOUNT;i++) {
      myrx.enableReceive( i );
      }
  }
}

void cmd_rxdisable(String param ){
    if ( param == "0" ) myrx.disableReceive( 0 );
    if ( param == "1" ) myrx.disableReceive( 1 );
    if ( param == "2" ) myrx.disableReceive( 2 );
    if ( param == "3" ) myrx.disableReceive( 2 );
    if ( param == "all" ) {
      for (int i=0;i<RADIOCOUNT;i++) {
        myrx.disableReceive( i );
      }
    }
}



void cmd_txframe(String param ){

RFframe f;
  f.period=150;
  //C aan y1313131313131313131133113311331133113313131313111
  //C uit y1313131313131313131133113311313133113311313133111
  //      SS
  //      00000000011111111112222222222333333333344444444445555555555  
  //      12345678901234567890123456789012345678901234567890123456789
  f.data="1913131313131313131311331133113131331133113131331111y";
  //f.data="193131313131313131311331133113131331133113131331131y";
  //f.data="3131313131313131313131313131131331313131131331311u";
//aan
//f.data="3131313131313131313113311331131313311331133113131v";
// 0 = 1313
// 1 = 3131
// 2 = 1331
// 3 = 3113?
f = impuls(0,'c',1);
int r = sendframe(f ,0 ); 
/*
String trits="111112202220";
// data
f.data = tritsToRel(trits);
// frame-end
f.data += "1v";
Serial.println(tritsToRel(trits));
Serial.println(f.data);
//1    1    1    1    1    2    2    0    2    2    2    0 
//3131 3131 3131 3131 3131 1331 1331 1313 1331 1331 1331 1313 1v

Serial.print(F("+SENDFRAME:"));
Serial.print( f.data );
Serial.println("#");
//Serial.print("+");
int r=sendframe(f,0);
//uit
//f.data="3131313131313131313113311331131313311331 1313 1331 1v";
//f.data="3131313131313131313113311331131313311331133113131v";
Serial.println(tritsToRel(trits));
Serial.println(f.data);
*/
String trits="111112202202";
// data
f.data = tritsToRel(trits);
// frame-end
f.data += "1v";

f = impuls(0,4,0);
Serial.println(f.data);

delay(2000);
 r=sendframe(f,0);
  
}








