/* 
 *  Params to set
 *  variable ref- ref x 4 secs = total delay in sampling
 *  Wire connection for mode change via pin A5
 *  sensor resolution set - to 11 bits
 *  baudrate - 1000000
*/
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
#include <avr/wdt.h>
#include <avr/sleep.h>

#define ONE_WIRE_BUS 9
#define noOfSensors 4

OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

volatile int count=0; //count then of time watchdog timer overflows, incremetn in ISR
int ref=225; //225: 15 of 4 sec triggers =1 min x 15 =15 mins

int loggingActive=0;

float sensorValueSample = 0;        // ADC value read from the sensor       

int eepromAddr=2;
int addr=2;
int temp=0;
//int memSelect;// when pin 12 connected to vcc after reset it will continue from last eeprom location, else if it is gnd it will start from zero of eeprom

void setup() {
  // initialize communications over RF
 // pinMode(0,INPUT_PULLUP);
  Serial.begin(1000000);
  pinMode(A5,INPUT); //for switching mode of the program
 // pinMode(8,OUTPUT); //only for connections
 // digitalWrite(8,HIGH);
  //pinMode(12,INPUT); 
  sensors.begin();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  //sensors.setResolution(insideThermometer, 11);
}

void loop() {
loggingActive=digitalRead(A5);
if(loggingActive){         //added eeprom length constriant
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
            sensorValueSample = sensors.getTempCByIndex(0);
  EEPROM.put(eepromAddr,sensorValueSample);
 eepromAddr += sizeof(float);
  #endif        
  #if noOfSensors>=2    
            sensorValueSample = sensors.getTempCByIndex(1);
  EEPROM.put(eepromAddr,sensorValueSample);
  EEPROM.put(0,eepromAddr);                             //-----------CHANGE AS IT CAN HOLD UPTO 255 ONLY
 eepromAddr += sizeof(float);
  #endif
  #if noOfSensors>=3                
            sensorValueSample = sensors.getTempCByIndex(2);
  EEPROM.put(eepromAddr,sensorValueSample);
  EEPROM.put(0,eepromAddr);     //EEPROM addr 0 stores last eeprom address written. Divide by 4 to see no. of logs.
 eepromAddr += sizeof(float);
#endif
#if noOfSensors>=4
            sensorValueSample = sensors.getTempCByIndex(3);
  EEPROM.put(eepromAddr,sensorValueSample);
  EEPROM.put(0,eepromAddr);                             //-----------CHANGE AS IT CAN HOLD UPTO 255 ONLY
 eepromAddr += sizeof(float);
#endif
#if noOfSensors>=5
            sensorValueSample = sensors.getTempCByIndex(4);
  EEPROM.put(eepromAddr,sensorValueSample);
  EEPROM.put(0,eepromAddr);                             //-----------CHANGE AS IT CAN HOLD UPTO 255 ONLY
 eepromAddr += sizeof(float);
#endif

  //below part eqv to 15 min delay
  //wdt_reset();
  watchdogSetup(1);   //enable wdt
  count=0;
  while(count<ref){
    sleep_mode(); //sleep for timer x ref seconds, here 225 secs = 15 mins
  }

 // if(((signed int16_t)eepromAddr)>((signed int16_t)EEPROM.length()-(signed int16_t)(noOfSensors*sizeof(float)))){  //check
    if(eepromAddr>1024-4*4){
    while(1)
      sleep_mode();
  }

}
  else{
  watchdogSetup(0); //disable wdt
  Serial.println("Enter 1");
  if(Serial.available()){    
    if(Serial.read()=='1'){
      EEPROM.get(0,temp);
      temp=1024;                                      //---------------------SEE MODIFICATION
    
      Serial.print("EEPROM addrs:");                                                  //remove
      Serial.println(temp);                                                           //remove
      addr=2;
      while(addr<=temp){                   
      
          EEPROM.get(addr,sensorValueSample);  
          Serial.println(sensorValueSample);
          addr += sizeof(float);
      
        }
      }
    }
  }
delay(2000);  
}

ISR(WDT_vect){//put in additional code here
  count++;
}

void watchdogSetup(bool input){
  cli();
  wdt_reset();
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  if(input==1)                                   //input 1 means set wdt, 0 means stop wdt
    WDTCSR = (1<<WDIE) | (0<<WDE) | (1<<WDP3);  // 4s / interrupt, no system reset
  else
    WDTCSR = (0<<WDIE) | (0<<WDE);  
  sei();
}
