#include <DAC.h>

const float refVoltage=5; //Set this value depending on exact input voltage from source (ie USB)
const int numDacs=4; //user input: how many DACS do you have?

float pInVoltage =0; // initialize inlet pressure
float pIn=0;

//float pAtm=101.8; //kPa -- this also should be extern, referenced in .cpp file in DAC::read() 

//must use these pins, unless also changing them in the header file -- how to use extern variables with arduino??
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

float cal1,cal2,cal3,cal4;


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

    cal1=ADC1.calibrate(0);
    cal2=ADC1.calibrate(1);
    cal3=ADC2.calibrate(0);
    cal4=ADC2.calibrate(1);
}

void loop() {
 //Uncomment following block to control all DACS at once
//MAKE THIS INTO ITS OWN "ALLDACS-OFF, ALLDACS-ON" CLASS -----
//  for (int i=0; i<numDacs; i++){
//      allDacs[i]->on(1.2345);
//    }

  digitalWrite(rSol, LOW); 
  digitalWrite(rT1, LOW);
  //DAC1.on(30);
  DAC2.on(25);
  DAC3.on(15);
  DAC4.on(10);
  ADC1.read(0,cal1);
  ADC1.read(1,cal2);
  ADC2.read(0,cal3);
  ADC2.read(1,cal4);
  Serial.print("\n");

  //inlet pressure  
  pInVoltage = analogRead(pRead)/1024.0*refVoltage;
  pIn=((pInVoltage/refVoltage+0.00842)/0.002421-101.8)*0.145037738; //change 101.8 to pAtm (needs to be extern variable)

  /*
  Serial.print("pIn: ");
  Serial.print(pIn,2);
  
  Serial.print("\n");
  */
}
