// code to test mem compression for eeprom storage
#include <EEPROM.h>

const uint8_t noOfSensors=2;
bool compression =0;  //mode, 0 is normal 1 is compression mode
uint8_t compress_startByte=0xFD;
uint8_t compress_endByte=0xFE;
uint8_t diffToWrite[noOfSensors]={0};
float sensor_prev[noOfSensors]={0};
float sensor_curr[noOfSensors]={0};
float sensor_diff[noOfSensors]={0};
bool threshold_diff = 1; //true means compress, diff is less than expected
uint16_t addrs=0;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  while(!Serial.available());

  threshold_diff=1;
  for(int i=0;i<noOfSensors;i++){
  sensor_curr[i]=Serial.parseFloat();
  Serial.read();
  sensor_diff[i]=sensor_curr[i]-sensor_prev[i];
  sensor_prev[i]=sensor_curr[i];
  
  if ((sensor_diff[i]>=0 & sensor_diff[i]<1.27) | (sensor_diff[i]<0 & sensor_diff[i]>-1.27))
    threshold_diff &= 1;
  else 
    threshold_diff &= 0;           
  }

  if(threshold_diff){
    if(!compression){
    compression=1;
    Serial.print(compress_startByte,HEX);
    EEPROM.put(addrs,compress_startByte);
    addrs+=sizeof(uint8_t);
    Serial.print(" ");
    }
      for(int i=0;i<noOfSensors;i++){
        diffToWrite[i]=(signed int8_t)(sensor_diff[i]*100);
        if(diffToWrite[i]==compress_startByte) diffToWrite[i] += 1; 
        if(diffToWrite[i]==compress_endByte) diffToWrite[i] += 1;
        Serial.print(diffToWrite[i]);
        EEPROM.put(addrs,diffToWrite[i]);
        addrs+=sizeof(signed int8_t);
        Serial.print(" ");
      }      
  }
  else{
    if(compression){
    compression=0;
    Serial.print(compress_endByte,HEX);
    EEPROM.put(addrs,compress_endByte);
    addrs+=sizeof(uint8_t);
    }
    for(int i=0;i<noOfSensors;i++){
        Serial.print(sensor_curr[i]);
        EEPROM.put(addrs,sensor_curr[i]);
        addrs+=sizeof(float);
        Serial.print(" ");
        }
  }
 
  //decompressoin
  //wait for eeprom addrs 10 and release decompressed state
  
  if(addrs>25){
  Serial.println(" ");
  bool flag=0;//same as compress flag
  for(int temp=0; temp<25;){
   if(flag==0){
    for(int i=0;i<noOfSensors;i++){
      EEPROM.get(temp,sensor_curr[i]);
      temp+=sizeof(float);
      Serial.print(sensor_curr[i]);
      Serial.print(" ");
      }
    }
    diffToWrite[0]=EEPROM.read(temp);
      if(diffToWrite[0]==compress_startByte){
        flag=1;
        temp+=1;
        continue;
        }
      if(diffToWrite[0]==compress_endByte){
        flag=0;
        temp+=1;
        continue;
        }  
      if(flag==1){
        for(int i=0;i<noOfSensors;i++){
          EEPROM.get(temp,diffToWrite[i]);
          temp+=sizeof(signed int8_t);
          if(diffToWrite[i]<0x80)
            sensor_curr[i] += ((float)diffToWrite[i])/100.0;
          else{ //convert negative int to float, default casting doesnt work
              uint8_t flipbits=0;
              flipbits |= (diffToWrite[i])^(0xFF);    //1s complement
              flipbits += 1;                            // add 1 to get 2s complement - the number to be subtracted
              sensor_curr[i] -= ((float)flipbits)/100.0;  // note minus sign
            } 
          Serial.print(sensor_curr[i]);
          Serial.print(" ");
          }
       }
             
    }
  }
    
  
  
}
