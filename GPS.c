/*
 * GPS.c
 *
 * Created: 3/15/2013 7:02:44 PM
 *  Author: Andre Lucas
 
 This is a sample program showing how to use the USART of the ATmega and parse some data. The program receives a transmission from the Locosys GPS LS20031 through TTL Asynchronous Serial communication,
 and retransmits only the NMEA GGA message to its serial line. Check the Locosys datasheet to see what the messages are. The GGA (Global positioning system fixed data) message contains, among other things, latitude and longitude data.
 The other messages being outputted by the GPS are simply ignored by the microcontroller.
 
 A usual transmission from the GPS looks like this:
 $GPGGA,064913.119,,,,,0,0,,,M,,M,,*48
 $GPGLL,,,,,064913.119,V,N*7A
 $GPGSA,A,1,,,,,,,,,,,,,,,*1E
 $GPGSV,1,1,00*79
 $GPRMC,064913.119,V,,,,,0.00,0.00,100180,,,N*45
 $GPVTG,0.00,T,,M,0.00,N,0.00,K,N*32
 
 Watching the serial output from the ATmega using putty, we can see the parsed message that looks like this:
 GPGGA,065039.520,,,,,0,0,,,M,,M,,*46
 
 */ 
#define F_CPU 20000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h> //Needed for printf function. (My favorite C function ;).
#include <string.h>

void ioinit(void);	//We're not actually using this since we don't use any pins for output, input. (The TX/RX pin for the USART are setup automatically when we turn that on.)
void USARTInit(unsigned int ubrr_value);	//Initializes the USART with the standard protocol required by the GPS.
char USARTReadChar(void);	//Reads a character from the buffer of the USART, which was received through the RX pin. Note that when you read the buffer it gets cleared and the next character received is written to it.
void USARTWriteChar(char data);	//Writes a character to the USART send buffer. The character will get sent as soon as possible.

static FILE mystdout = FDEV_SETUP_STREAM(USARTWriteChar, NULL, _FDEV_SETUP_WRITE); //Don't worry too much about this line. It is needed for the printf function. It is basically declaring a stream for the printf to use, which just uses the USARTWriteChar function.

int main(void){
	char data=0;
	char str[60]; //Strings in C are just arrays of char types. For example, char ex[2]="Hi" has ex[0]='H', ex[1]='i' and ex[2]='\0' or Null, which is 0. The Null character represents the end of a string. Just treat them as arrays.
	
	ioinit();
	USARTInit(21); //The value 21 is the UBBR value that will set the baud rate of the USART. The GPS is running at 56700 bps so that is what this value of 21 will give us. Check the ATmega's datasheet under USART for more info on this.
	
    while(1){
			
		data=USARTReadChar();	
		if (data=='$'){			//The while loop just goes around until the dollar sign is received. The dollar sign is preceded by every message of the GPS. (Check the syntax of the messages on the locosys datasheet).
			
			for (char i=0;i<5;i++){		//The first 5 characters after a dollar sign is for the message ID. In our case we only want the GGA messages, that is, GPGGA. So we write the first 5 characters after the dollar sign to a string.
				
				data=USARTReadChar();
				str[i]=data;
			}
			
			str[5]='\0';	//In case this is not the first time through this loop, the string might contain text from the previous run but we need the string to be clear again with only the 5 characters we just wrote to it. So we write the Null character right after the 5th character. Now the space after it will be seen as empty.
			
			if (strcmp(str, "GPGGA") == 0){ //Here we compare the string with the message ID we are looking for, GPGGA. If we hadn't written the Null character to the string just above, this comparison would only work during the first run through the while loop.
				
				for (char i=5;data!=10;i++)	//This for loop writes the whole message to the string. It stops when it reaches the newline feed character, which is the end of the message from the GPS.
				{
					data=USARTReadChar();
					str[i]=data;
				}
				
				printf(str); //We print the GPGGA message to the serial output.
			}
		}
    }
}

void ioinit(void){
	// No need to setup GPIO
}

void USARTInit(uint16_t ubrr_value){

	//Set Baud rate. Again, the ATmega datasheet has a sample baud rate table for ATmega's running at several frequencies and what UBBR value should be. For asynchronous serial Baud rate=fosc/16(UBBR + 1). For UBBR=21 and fosc=20Mhz, we get 56818 bps... pretty close.
   
	UBRR0L = ubrr_value;
	UBRR0H = (ubrr_value>>8);

	/*Set Frame Format. This is the default for Asynchronous serial and it's the protocol the GPS uses.


	>> Asynchronous mode
	>> No Parity
	>> 1 StopBit

	>> char size 8
	*/
   
	UCSR0C= (1<<UCSZ00)|(1<<UCSZ01); //8-bit frame


	UCSR0B=(1<<RXEN0)|(1<<TXEN0); //Enable The receiver and transmitter

	stdout = &mystdout; //Sets the stream we created at the top of the code to be the standard stream out. So by default printf function will send chars to this string.

}

char USARTReadChar()
{
   //Wait until data is available

   while(!(UCSR0A & (1<<RXC0)))
   {
      //Do nothing
   }

   //Now USART has got data from device and is available inside the buffer.

   return UDR0;
}

void USARTWriteChar(char data)
{
   //Wait until the transmitter is ready. (This is important if, for example, the USART was sending data out from the buffer... we can't just write the buffer in the middle of a transmission.)

   while(!(UCSR0A & (1<<UDRE0)))
   {
      //Do nothing
   }

   //Now write the data to USART buffer

   UDR0=data;
}