#ifndef DAC_h
#define DAC_h

#include "Arduino.h"

class DAC
{
	public:
		DAC (int SS, float refVoltage);
		//DAC functions
		void on(float outputPressure);
		void off();
		//ADC functions
		void read(byte channel, float calibrate);
		float calibrate(byte channel);
		//void pressureCheck

	private:
		int _SS;
		float _refVoltage;
		float _cal;
		
		float inc;
		word output;
		
		const int SS1=22; 
		const int SS2=24;
		const int SS3=26;
		const int SS4=28;
		const int SS5=30;
		const int SS6=32;
};

/* class ADC
{
	public:
		ADC (int a);
		void test(float hello);
};	 */	

class relay
{
	public:
		relay (int selPin);
		void on ();
		void off ();
	private:
		int _selPin;
};
#endif
