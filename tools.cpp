#include "rftrx.h"


int freeRam() {
//  extern int __heap_start, *__brkval; 
//  int v; 
 // return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
return 0;
}


int frameCharToInt( char c ) {
  if ( c < 'z' ) { 
  if ( c > '9') {
    if ( c > 'Z') {
     return c - 'a' + 10;
 
    }else {
    
      return c - 'A' + 10;
    }
  } else {
    return c-'0';
  }
  }
  
}

char frameIntToChar( int p ) {
                                  // moet 0-9a-z worden om de 31x op te vangen.
                                if ( p > 35 ) p=35;
                                if (p < 10) {
                                 return p;
                                }else{
                                  return 'a'+p-10;
                                }
}

String tritsToRel( String trits){
  // 0 = 1313
// 1 = 3131
// 2 = 1331
// 3 = 3113?
  String ret="";
  for (int i=0;i<trits.length();i++) {
    if ( trits.charAt(i) == '0') ret += "1313";
    if ( trits.charAt(i) == '1') ret += "3131";
    if ( trits.charAt(i) == '2') ret += "1331";
    if ( trits.charAt(i) == '3') ret += "3113";
  }
  return ret;  
}

