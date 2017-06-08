#include <DAC.h>

const float refVoltage=4.26; //Set this value depending on exact input voltage from source (ie USB)
const int numDacs=4; //user input: how many DACS do you have?

float pIn =0; // initialize inlet pressure

//must use these pins, unless also changing them in the header file
//DAC pins
const int SS1=22;
const int SS2=24;
const int SS3=26;
const int SS4=28;
//ADC pins
const int SS5=30;
const int SS6=32;

//built in ADC pin
const int pRead = 1; //analog input

//DAC name (pinNumber, referenceVoltage)
DAC DAC1 (SS1,refVoltage);
DAC DAC2 (SS2,refVoltage);
DAC DAC3 (SS3,refVoltage);
DAC DAC4 (SS4,refVoltage);

//ADC name (pinNumber, referenceVoltage) 
DAC ADC1 (SS5, refVoltage);
DAC ADC2 (SS6, refVoltage);

//to control using allDacs
DAC * allDacs [4];
DAC * objectPointer;

//Relay pins
const int rSol = 2; //solenoid valve
const int rT1 = 3; //transducer pin




void setup() {

  //to control using allDacs
  for (int i=0; i<numDacs; i++){
    objectPointer = new DAC (SS1+2*i, refVoltage);
    allDacs[i]= objectPointer;
  }

  pinMode(rSol, OUTPUT);  //initialize solenoid valve pin
  
  //initialized transducers pins
  pinMode(rT1, OUTPUT);   

  pinMode (pRead, INPUT);
  
   
Serial.begin(250000);
}

void loop() {
 //Uncomment following block to control all DACS at once
//MAKE THIS INTO ITS OWN "ALLDACS-OFF, ALLDACS-ON" CLASS -----
//  for (int i=0; i<numDacs; i++){
//      allDacs[i]->on(1.2345);
//    }

  digitalWrite(rSol, LOW); 
  digitalWrite(rT1, LOW);
  DAC1.off();
  DAC2.on(3.6);
  DAC3.off();
  DAC4.on(0);
  ADC1.read(0);
  ADC1.read(1);
  ADC2.read(0);
  ADC2.read(1);

  pIn = analogRead(pRead);
  //inlet pressure
  Serial.print("pIn: ");
  Serial.print(pIn);
  
  Serial.print("\n");
}
