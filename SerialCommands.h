#ifndef _SERIAL_COMMANDS2_H
#define _SERIAL_COMMANDS2_H

#include <Arduino.h>



//#include "commands.h"

void serialInit();
// serial

void func ( void (*f)(int ) );
extern String inputString;         // a string to hold incoming data
extern boolean stringComplete;  // whether the string is complete



void serialEvent();


// --------------------------------------------------------
// Standard Includes
// --------------------------------------------------------
//#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <inttypes.h>
//#include <String>
//
//#include <Wire.h>

typedef void (*voidFunction) (void);
typedef void (*functiontype)();
typedef void (*voidStringArrayFunction) (String param[],int paramCount);
typedef void (*voidStringFunction) (String param);
typedef int (*intStringFunction) (String param);
typedef void (*voidIntFunction) (int param);

typedef struct  {
  String name;
  voidStringFunction func;
  int arguments;
  String help;
} Command;

typedef struct  {
  String name;
  intStringFunction func;
  int arguments;
  String help;
} intCommand;

void parseCommand(String cmd, String param);

void help(String param );
void cmd_version(String param );
void cmd_showradios(String param );
void ping(String param );

void cmd_rxenable(String param);
void cmd_rxdisable(String param);


/*

*/
#endif
