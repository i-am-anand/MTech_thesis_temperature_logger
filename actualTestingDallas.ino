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
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

#define ONE_WIRE_BUS 4
#define noOfSensors 5

OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

int loggingActive=0;

int sensorValueSample = 0;        // ADC value read from the sensor       

int eepromAddr=2;
int addr=2;
int temp=0;
//int memSelect;// when pin 12 connected to vcc after reset it will continue from last eeprom location, else if it is gnd it will start from zero of eeprom

void setup() {
  // initialize communications over RF
  Serial.begin(9600);
  pinMode(2,INPUT); //for switching mode of the program
  pinMode(8,OUTPUT); //only for connections
  digitalWrite(8,HIGH);
  //pinMode(12,INPUT); 
  sensors.begin();
}

void loop() {
loggingActive=digitalRead(2);
  if(loggingActive && eepromAddr<(EEPROM.length()-12)){         //added eeprom length constriant
/*           memSelect=digitalRead(12);
    if(memSelect){
      eepromAddr=EEPROM.read(0);
      eepromAddr++;
      eepromAddr++;
     }
    else
      eepromAddr=1;  */
  sensors.requestTemperatures(); // Send the command to get temperatures
  delay(10);
  
  #if noOfSensors>=1        
            sensorValueSample = (int)sensors.getTempCByIndex(0);
  EEPROM.put(eepromAddr,sensorValueSample);
 eepromAddr += sizeof(int);
  #endif        
  #if noOfSensors>=2    
            sensorValueSample = (int)sensors.getTempCByIndex(1);
  EEPROM.put(eepromAddr,sensorValueSample);
  EEPROM.put(0,eepromAddr);                             //-----------CHANGE AS IT CAN HOLD UPTO 255 ONLY
 eepromAddr += sizeof(int);
  #endif
  #if noOfSensors>=3                
            sensorValueSample = (int)sensors.getTempCByIndex(2);
  EEPROM.put(eepromAddr,sensorValueSample);
  EEPROM.put(0,eepromAddr);     //EEPROM addr 0 stores last eeprom address written. Divide by 4 to see no. of logs.
 eepromAddr += sizeof(int);
#endif
#if noOfSensors>=4
            sensorValueSample = (int)sensors.getTempCByIndex(3);
  EEPROM.put(eepromAddr,sensorValueSample);
  EEPROM.put(0,eepromAddr);                             //-----------CHANGE AS IT CAN HOLD UPTO 255 ONLY
 eepromAddr += sizeof(int);
#endif
#if noOfSensors>=5
            sensorValueSample = (int)sensors.getTempCByIndex(4);
  EEPROM.put(eepromAddr,sensorValueSample);
  EEPROM.put(0,eepromAddr);                             //-----------CHANGE AS IT CAN HOLD UPTO 255 ONLY
 eepromAddr += sizeof(int);
#endif

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
      
          EEPROM.get(addr,sensorValueSample);  
          Serial.println(sensorValueSample);
          addr += sizeof(int);
      
        }
      }
    }
  }
      
  
delay(2000);  
}
