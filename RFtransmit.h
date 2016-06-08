#ifndef _RFTRANSMIT_H
#define _RFTRANSMIT_H


void setupTX();
int sendframe(RFframe &f,int channel,int repeat=1);


#endif
