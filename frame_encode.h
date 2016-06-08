  
#ifndef _FRAME_ENCODE_H
#define _FRAME_ENCODE_H

#include "rftrx.h"


RFframe impuls(int address, char c, int action);
RFframe impuls(int address, int code, int action);


RFframe elro(int address, char c, int action);
RFframe elro(int address, int code, int action);




#endif

