#include "Arduino.h"
#include "DAC.h"
#include "SPI.h"

//need to make the code better - class of classes so all chips on, off at once, etc using pointers and new class for adc maybe? but referencing DAC so they are both looking at the same code and then ADC adds some new code for it's own purposes, etc. how do you do this?
DAC::DAC(int SS, float refVoltage)
{
	//Set the pins used to slave select each chip

	_SS=SS;
	_refVoltage=refVoltage;
	
	//DAC pins
	pinMode (SS1, OUTPUT);
	pinMode (SS2, OUTPUT);
	pinMode (SS3, OUTPUT);
	pinMode (SS4, OUTPUT);
	//ADC pins
	pinMode (SS5, OUTPUT);
	pinMode (SS6, OUTPUT);	
	
	inc = _refVoltage/4096; 
	
	digitalWrite(SS1, HIGH); 
	digitalWrite(SS2, HIGH);
	digitalWrite(SS3, HIGH);
	digitalWrite(SS4, HIGH);
	
	digitalWrite(SS5, HIGH);
	digitalWrite(SS6, HIGH);
	

	
	SPI.begin();
	
}

void DAC::on(float outputPressure)
{
	byte data=0;
	
	float outputVoltage=0;
	
	outputVoltage=outputPressure/6.0;
	
	if (outputVoltage>=_refVoltage)	//Since we can't output a voltage to the dac greater than the reference voltage,
									//if the user asks for a voltage that is impossible, it sets the output to 0
	{
		outputVoltage=0;
	}
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


void DAC::read(byte channel, float calibrate) //channel 0 or 1
{
	byte vinMSB=0;
	byte vinLSB=0;
	word conca=0;
	float adcOut=0;
	float convertedVoltage=0;
	byte _channel=0;
	float readVoltage=0;
	
	float pGauge=0;
	
	SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
	
	digitalWrite(_SS,LOW);
	
	_channel=(channel&0B00000001)<<6;
	
	byte configBits=0B10100000|_channel;
	
	SPI.transfer (B00000001); //start bit
	vinMSB=0B00001111&SPI.transfer(configBits); //first real transfer out contains configuration, transfer in contains 4 useful bits 
	vinLSB=SPI.transfer(0x00); //all 8 bits contain useful information
	conca=((vinMSB<<8)&0B1111111100000000)+vinLSB;
	adcOut=(float)conca;
	
	readVoltage=inc*adcOut; //this variable exists only to show pre-calibrated values - to see which pressure sensors are working
	
	convertedVoltage=inc*adcOut+calibrate; //take the voltage read by the dac and add the calibration value
	
	//pGauge = ((convertedVoltage-0.95)/_refVoltage)*30;
	pGauge = (((convertedVoltage)/_refVoltage+0.00842)/0.002421-101.8)*0.145037738; //gauge pressure using transfer function of MPXH6400A pressure sensor, subtract atmospheric pressure, convert to psi
																				  //should use pAtm (External variable) instead of 101.8
	
	//Serial.begin(9600);
	
	Serial.print("[");
	Serial.print(_SS);
	Serial.print(" ch");
	Serial.print(channel);
	Serial.print("] rd=");
	Serial.print(readVoltage,4);
	Serial.print(" cal=");
	Serial.print(convertedVoltage, 2);
	Serial.print(" Pg=");
	Serial.print(pGauge,4);
	Serial.print("\t");	
	
	digitalWrite(_SS, HIGH);
	
	SPI.endTransaction();

}

//calibrate function takes many values (see int setCal) from ADC at atm and averages them out. Should read 1.19V at atmospheric pressure, so determines a calibration value based on this.
float DAC::calibrate(byte channel)
{
	
	//note: unsure proper way of using functions within class, so manual copy and pasted most code from DAC::read with some slight changes -- I am sure there is a more effective way of doing this.
	
	byte vinMSB=0;
	byte vinLSB=0;
	word conca=0;
	float adcOut=0;
	
	
	const int setCal=10; //number of calibration readings
	float convertedVoltage[setCal]={0};
	byte _channel=0;
	
	float pGauge=0;
	float sum=0;
	float avgV=0;
	
	
	SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
	

	
	for (int y=0; y<setCal; y++){		//reads ADC values as many times as determined by setCal and averages the values after
		
		digitalWrite(_SS,LOW);
	
		_channel=(channel&0B00000001)<<6;
	
		byte configBits=0B10100000|_channel;
		SPI.transfer (B00000001); //start bit
		vinMSB=0B00001111&SPI.transfer(configBits); //first real transfer out contains configuration, transfer in contains 4 useful bits 
		vinLSB=SPI.transfer(0x00); //all 8 bits contain useful information
		conca=((vinMSB<<8)&0B1111111100000000)+vinLSB;
		adcOut=(float)conca;
		
		convertedVoltage[y]=inc*adcOut;
		
		sum+=convertedVoltage[y];
		
		digitalWrite(_SS, HIGH);
		
		Serial.print(y);
		Serial.print("\t");
		Serial.print(convertedVoltage[y]);
		Serial.print("\n");
	}
	avgV=sum/setCal; 
	
		//example: if DAC reads 0.9V at atmospheric when it should read 1.19V, _cal=1.10 ---- this value is added to the reading 
	
	

	_cal=(_refVoltage*(0.002421*101.8-0.00842))-avgV; 		//(mpx_sensor transfer function solving for output voltage, using 101.8 as the given pressure (atmospheric) and finding the difference between that value and the avg calibration value
															//add this value to voltages in dac::read
	SPI.endTransaction();
	
	
	return _cal;
}

//int DAC::pressureCheck{

//}

relay::relay(int selPin){
	_selPin=selPin;
	
}
void relay::on(){
	digitalWrite(_selPin, LOW);
}
void relay::off(){
	digitalWrite(_selPin, HIGH);
}

	
/* ADC::ADC (int a)
{
	
}
void ADC::test(float hello)
{
	
}
	 */
