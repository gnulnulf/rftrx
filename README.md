# rftrx 
universal arduino 433/868/ir pulse receiver and optional decoding

inspired by a lot but especially:
https://bitbucket.org/fuzzillogic/433mhzforarduino/wiki/Home
http://www.nodo-domotica.nl/
http://www.nemcon.nl/blog2/

Based on the idea pulses are relative to each other and protocols are quite the same for multiple vendors.

Three radios can be attached to the arduino mega and will be monitored with interrupts.
when a frame is received the following will happen:
- find the smallest usefull pulse.
- create a string of alphadecimal [0-9][a-z] of relative pulselengths.
- detect lower encoding ( spacelen, 123spacelen, manchester, tristate )
- based on the lower encoding, find the corresponding sender protocol.
- validate based on zero-mask, one-mask, same-mask(s)
	....----0101....----aa => 0101 should be an axact match
	 all 'a's should be the same, all other chars are convenient for counting
- decode and show as json, rflink, raw txt
	the decode string should be something like:
	----aaaabbccccccccccc--
	this will create an associative array, probably with signing and crc etc.

a frame could be send by sending  strings containing relative pulses and extra information like pulselength, repeats etc.
 

usage
=====
At this point all is alpha and probably not working like expected.
I use the arduino-mk to compile and upload this, mainly from a rasberry pi.

make
make upload

To view from the command line I use:
screen -F /dev/ttyUSB0 57600
