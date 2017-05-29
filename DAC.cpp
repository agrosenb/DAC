#include "Arduino.h"
#include "DAC.h"
#include "SPI.h"

//need to make the code better - class of classes so all chips on, off at once, etc using pointers and new class for adc maybe? but referencing DAC so they are both looking at the same code and then ADC adds some new code for it's own purposes, etc. how do you do this?
DAC::DAC(int SS, float refVoltage)
{
	//Set the pins used to slave select each chip

	_SS=SS;
	
	//DAC pins
	pinMode (SS1, OUTPUT);
	pinMode (SS2, OUTPUT);
	pinMode (SS3, OUTPUT);
	pinMode (SS4, OUTPUT);
	//ADC pins
	pinMode (SS5, OUTPUT);
	pinMode (SS6, OUTPUT);
	
	inc = refVoltage/4096; //should it be 4096 or 4095?
	
	digitalWrite(SS1, HIGH); 
	digitalWrite(SS2, HIGH);
	digitalWrite(SS3, HIGH);
	digitalWrite(SS4, HIGH);
	
	digitalWrite(SS5, HIGH);
	digitalWrite(SS6, HIGH);
	
	SPI.begin();
	
}

void DAC::on(float outputVoltage)
{
	byte data=0;

	output=(word)(outputVoltage/inc);
	digitalWrite(_SS,LOW);
	
	SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
	
	//writeHightByte stuff
	data = highByte(output);
	data = 0b00001111 & data;
	data = 0b00110000 | data;
	SPI.transfer(data);
	data=lowByte(output);
	SPI.transfer(data);
	
	digitalWrite(_SS, HIGH);
	
	SPI.endTransaction();
}

void DAC::off()
{
	//the same as DAC.on(0)
	
	digitalWrite(_SS,LOW);
	
	SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
	
	SPI.transfer(0b00110000);
	SPI.transfer(0);
	
	digitalWrite(_SS, HIGH);
	
	SPI.endTransaction();
}


void DAC::read(byte channel) //channel 0 or 1
{
	byte vinMSB=0;
	byte vinLSB=0;
	word conca=0;
	float adcOut=0;
	float convertedVoltage=0;
	byte _channel=0;
	
	SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
	
	digitalWrite(_SS,LOW);
	
	_channel=(channel&0B00000001)<<6;
	
	byte configBits=0B10100000|_channel;
	
	SPI.transfer (B00000001); //start bit
	vinMSB=0B00001111&SPI.transfer(configBits); //first real transfer out contains configuration, transfer in contains 4 useful bits 
	vinLSB=SPI.transfer(0x00); //all 8 bits contain useful information
	conca=((vinMSB<<8)&0B1111111100000000)+vinLSB;
	adcOut=(float)conca;
	
	convertedVoltage=inc*adcOut;
	
	//Serial.begin(9600);
	
	Serial.print("[mcp");
	Serial.print(_SS);
	Serial.print(" ch");
	Serial.print(channel);
	Serial.print("]: ");
	Serial.print(convertedVoltage, 6);
	Serial.print("\t");	
	
	digitalWrite(_SS, HIGH);
	
	SPI.endTransaction();

}
	
/* ADC::ADC (int a)
{
	
}
void ADC::test(float hello)
{
	
}
	 */