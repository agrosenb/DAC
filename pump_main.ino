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

//LED pins
const int ledG=6;
const int ledY=7;

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
const int rSolPin = 2; //solenoid valve
const int rTPin = 3; //transducer pin

//relay NAME (pin)
relay solenoid (rSolPin);
relay transducers (rTPin);
  


float cal1,cal2,cal3,cal4;  //calibration variables

//**this section is temporary to test the analog read functionality
float testRead=0;
int testPin=9;
//**


void setup() {
  
  //to control using allDacs
  for (int i=0; i<numDacs; i++){
    objectPointer = new DAC (SS1+2*i, refVoltage);
    allDacs[i]= objectPointer;
  }

  pinMode(rSolPin, OUTPUT);  //initialize solenoid valve pin
  
  pinMode(rTPin, OUTPUT);   //initialized transducers pins
  pinMode (pRead, INPUT);   //initialize inlet pressure reading (from analog in pin built into board)

  pinMode (testPin, INPUT); //**temporary to test analog read values

  pinMode (ledY, OUTPUT);
  pinMode (ledG, OUTPUT);

  digitalWrite(ledG, LOW);
  digitalWrite(ledY, LOW);

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

  solenoid.on();
  pIn=readInlet(); //returns psi of inlet valve
  
  if (pIn>41){  //if the inlet pressure is greater than 41 psi, turn on the transducer relays
    transducers.on();
    digitalWrite(ledG, HIGH);
    digitalWrite(ledY, LOW);

  //testDAC();
  
  //DACX.on(psi_value) or DAC.off()
  DAC1.off();
  DAC2.on(10);
  DAC3.on(20);
  DAC4.on(28);

  }else{
    transducers.off();
    digitalWrite (ledG, LOW);
    digitalWrite (ledY, HIGH);
    delay (1000);
    digitalWrite (ledY, LOW);
    delay(1000);
  }
  
  //ADCX.read(channel, calibration_value)
  //ADC1.read(0,cal1);
  ADC1.read(1,cal2);
  ADC2.read(0,cal3);
  ADC2.read(1,cal4);
  Serial.print("\n");


  
  Serial.print("pIn: ");
  Serial.print(pIn,2);
  
  Serial.print("\n");
  
}



float readInlet (){
  pInVoltage = analogRead(pRead)/1024.0*refVoltage; //using built-in MEGA analog ADC therefore only 10 bit (1024)
  pIn=((pInVoltage/refVoltage+0.00842)/0.002421-101.8)*0.145037738; //gauge pressure using transfer function of MPXH6400A pressure sensor, subtract atmospheric pressure, convert to psi 
                                                                    //change 101.8 to pAtm (needs to be extern variable)

  return pIn;
}
void testDAC(){
     float stp=5.0/4096.0; //min voltage increment of 12 bit DAC
     
     for (float i=0; i<5; i=i+stp*10){
       DAC2.on(6*i); //6*[voltage value] gives the psi output that is required by the function.
       delay(50); 
       DAC2.off();
       delay(10); 
      }
   
}

