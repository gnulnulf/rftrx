
#include "rftrx.h"


/*
 * KAKU IMPULS
 */

// create frame for impuls with button character for the code
RFframe impuls(int address, char c, int action){
  int code=0;
  if ( ( c >= 'a' ) && (c <= 'e')) {
    code = 1<<(c - 'a');
  } else  if ( ( c >= 'A' ) && (c <= 'E')) {
    code = 1<<(c - 'A');
  } else {
    code=0;
  }
  return impuls(address,code,action);
}



//Create a frame for the impulse remote.
RFframe impuls(int address, int code, int action){
/*
Serial.print("IMPULS:");
Serial.print(address);
Serial.print(" code:");
Serial.print(code);
Serial.print(" action:");
Serial.println(action);
*/

  RFframe r;
  r.period=150;
  r.data="";
  String trits="";

  //invert
  address = ~address;
  for (int i=4;i>=0;i--) {
    trits += (( address >> i)&1)?"1":"0";
  }

  //invert
  code = ~code;

  for (int i=4;i>=0;i--) {
    trits += (( code >> i)&1)?"2":"0";
  }

//uit
if ( action==0){
  trits+="02";
}
//aan
if ( action==1){
  trits+="20";
}

// data
r.data = tritsToRel(trits);
// frame-end
r.data += "1v";

/*
Serial.print("TRITS:");
Serial.println(trits);
Serial.print("IMPULSFRAME:");
Serial.println(r.data);
*/
 return r;
  
}

