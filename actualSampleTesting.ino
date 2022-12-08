/* Code for TWO lm35 sensors temperature value measurement & store in EEPROM & optionally send it over XBee/BT
Two LM35 sensor on anaolog pins A1 & A4, vcc and gnd. output pin has a bypas 1k resistor + 104capacitor series network going to gnd.

pin 2 - decides whether sensor sampling is active or not - pin 2 connected to vcc:active sampling, connected to gnd: stop sampling and logging
        if pin 2 is gnd & serial is available, program will change to reading eeprom over serial comm
pin 8 - permanently high, for connections
pin A0 - permanently high
pin A1 - sample's sensor input
pin A2 - permanently low
pin A3 - permanently high
pin A4 - ambient sensor input
pin A5 - permanently low

overall system - arduino kept in a closed box, powered by power bank/9V battery. Sensors and wires come out of box. One sensor for ambient, and one sticked to Aluminium foil of sample
to be tested. Antennas of wireless transceivers, if present, inside the box. Arduino will take temperature measurement from both sensors after every 30 min and store in EEPROM 
and may also send it over wireless channel.

NEED to NOTE the time when pin 2 is connected to VCC for timing log-------------------------------------
*/

#include <EEPROM.h>
const int sensorAmbient = A3;  // Analog input pin that the ambient LM35 temp sensor is attached to
const int sensorSample1= A4; 
const int sensorSample2= A5; //  Analog input pin that the sample's LM35 temp sensor is attached to

int loggingActive=0;

int sensorValueAmbient = 0;        // ADC value read from the sensor
int sensorValueSample1 = 0;        // ADC value read from the sensor
int sensorValueSample2 = 0;        

float analogVoltage=0;
float temperature=0;

int eepromAddr=2;
int addr=2;
int temp=0;
//int memSelect;// when pin 12 connected to vcc after reset it will continue from last eeprom location, else if it is gnd it will start from zero of eeprom

void setup() {
  // initialize communications over RF
  Serial.begin(9600);
  pinMode(2,INPUT); //for switching mode of the program
//  pinMode(12,INPUT); 
  pinMode(8,OUTPUT); //only for connections
  digitalWrite(8,HIGH);

// pinMode(A1,INPUT);
    pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  pinMode(A5,INPUT);
  
  
   analogReference(INTERNAL);                      // -------------------------------MAJOR


}

void loop() {
loggingActive=digitalRead(2);
  if(loggingActive){  
/*           memSelect=digitalRead(12);
    if(memSelect){
      eepromAddr=EEPROM.read(0);
      eepromAddr++;
      eepromAddr++;
     }
    else
      eepromAddr=1;  */
  // read the ambient analog in value:
  sensorValueAmbient = analogRead(sensorAmbient);
  EEPROM.put(eepromAddr,sensorValueAmbient);
  eepromAddr++;
  eepromAddr++;
  sensorValueSample1 = analogRead(sensorSample1);
  EEPROM.put(eepromAddr,sensorValueSample1);
  EEPROM.put(0,eepromAddr);                             //-----------CHANGE AS IT CAN HOLD UPTO 255 ONLY
  eepromAddr++; 
  eepromAddr++;
  sensorValueSample2 = analogRead(sensorSample2);
/*  EEPROM.put(eepromAddr,sensorValueSample2);
  EEPROM.put(0,eepromAddr);     //EEPROM addr 0 stores last eeprom address written. Divide by 4 to see no. of logs.
  eepromAddr++; 
  eepromAddr++; */
  //send over radio
  delay(180000); // delay(1799998); //15 mins delay including entire loop delay COMPENSATE EXTRA DELAY AT the END   //make dealy 1799998
  }
  else{
  if(Serial.available()){
    Serial.print("Enter 1 to print values");
    if(Serial.read()=='1'){
      EEPROM.get(0,temp);
      //temp=temp+260;                                      //---------------------SEE MODIFICATION
      //Serial.println("Temp values 15 mins log, ambient sample alternate:");
      Serial.print("EEPROM addrs:");                                                  //remove
      Serial.println(temp);                                                           //remove
      addr=2;
      while(addr<=temp){                   
      
          EEPROM.get(addr,sensorValueAmbient);  
          analogVoltage = (float)sensorValueAmbient*1.1/1024;       //set value according to AREF---------------------------------
          temperature=100*analogVoltage; 
         // Serial.print(" ");
          Serial.print(sensorValueAmbient);
          Serial.print("\t ");
          Serial.print(analogVoltage,1);
          Serial.print("\t ");
          Serial.println(temperature,1);
          addr += sizeof(int);
          
          EEPROM.get(addr,sensorValueSample1);  
          analogVoltage = (float)sensorValueSample1*1.1/1024;       //set value according to AREF---------------------------------
          temperature=100*analogVoltage; 
      //    Serial.print("analogValue = ");
          Serial.print(sensorValueSample1);
          Serial.print("\t ");
          Serial.print(analogVoltage,1);
          Serial.print("\t ");
          Serial.println(temperature,1);
          addr += sizeof(int);

/*          EEPROM.get(addr,sensorValueSample2);  
          analogVoltage = (float)sensorValueSample2*1.1/1024;       //set value according to AREF---------------------------------
          temperature=100*analogVoltage; 
        //  Serial.print("analogValue = ");
          Serial.print(sensorValueSample2);
          Serial.print("\t ");
          Serial.print(analogVoltage,1);
          Serial.print("\t ");
          Serial.println(temperature,1);
          addr += sizeof(int);      */   
        }
      }
    }
  }
      
  
delay(2000);  
}