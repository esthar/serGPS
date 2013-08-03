serGPS
===========

Amherst College Electronics Club: ATmega328 sample program for using the UART

This is a sample program showing how to use the UART of the ATmega and parse some data. The program receives a transmission from the Locosys GPS LS20031 through TTL Asynchronous Serial communication, and retransmits only the NMEA GGA message to its serial line. Check the Locosys datasheet to see what the messages are. The GGA (Global positioning system fixed data) message contains, among other things, latitude and longitude data.
The other messages being outputted by the GPS are simply ignored by the microcontroller.
 
A usual transmission from the GPS looks like this:

 $GPGGA,064913.119,,,,,0,0,,,M,,M,,*48
 $GPGLL,,,,,064913.119,V,N*7A
 $GPGSA,A,1,,,,,,,,,,,,,,,*1E
 $GPGSV,1,1,00*79
 $GPRMC,064913.119,V,,,,,0.00,0.00,100180,,,N*45
 $GPVTG,0.00,T,,M,0.00,N,0.00,K,N*32
 
Watching the serial output from the ATmega using putty, we can see the parsed message, which looks like this:

GPGGA,065039.520,,,,,0,0,,,M,,M,,*46
