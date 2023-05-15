#include <avr/wdt.h>
#include <avr/sleep.h>
int count=0;
int ref=8; //15 of 4 sec triggers =1 min x 15 =15 mins

void setup() {
  // put your setup code here, to run once:
 pinMode(LED_BUILTIN, OUTPUT);
  watchdogSetup();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  digitalWrite(LED_BUILTIN, HIGH);    
}

void loop() {
  // put your main code here, to run repeatedly:

  //delay(5000);                       // wait for a second
  wdt_reset();
  while(count<ref){
  sleep_mode();
  }
  count=0;
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  

//  delay(1000);                       // wait for a second
  wdt_reset();
 while(count<ref){
  sleep_mode();
  }
  count=0;
  digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage LOW
  
}

ISR(WDT_vect){//put in additional code here
  count++;
}

void watchdogSetup(void){
  cli();
  wdt_reset();
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  WDTCSR = (1<<WDIE) | (0<<WDE) | (1<<WDP2) | (0<<WDP1);  // 1s / interrupt, no system reset
  sei();
}
