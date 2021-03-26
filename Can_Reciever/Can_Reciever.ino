
#include <SPI.h>              //Library for using SPI Communication 
#include <mcp2515.h>          //Library for using CAN Communication
#include <Wire.h> 
int step1 = 7;
int step2 = 6;
int step3 = 5;
int step4 = 4; 
int step_number = 0;

struct can_frame canMsg; 

MCP2515 mcp2515(10);                 // SPI CS Pin 10 
 
void setup() {

  pinMode(step1,OUTPUT);
  pinMode(step2,OUTPUT);
  pinMode(step3,OUTPUT);
  pinMode(step4,OUTPUT);  
  SPI.begin();                       //Begins SPI communication
  
  Serial.begin(9600);                //Begins Serial Communication at 9600 baudrate 
  
  mcp2515.reset();      
  mcp2515.setBitrate(CAN_500KBPS,MCP_8MHZ); //Sets CAN at speed 500KBPS and Clock 8MHz 
  mcp2515.setNormalMode();                  //Sets CAN at normal mode
}

void loop() 
{
  
  //CAN BUS
  
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) // To receive data (Poll Read)
  {
    
    Serial.println("Made it in!");
    if(canMsg.can_id==0x036)
    {
     int x = canMsg.data[0];         
     Serial.println(x);
     if(x < 128){
      OneStep(true);
      delay(x);
     }
     else{
      OneStep(false);
      delay(255 - x);
     }
    }
   
}                    
}//FOR CANBUS


void OneStep(bool dir){

if(dir){

switch(step_number){
  case 0:
  digitalWrite(step1,HIGH);
  digitalWrite(step2,LOW);
  digitalWrite(step3,LOW);
  digitalWrite(step4,LOW);
  break;
  case 1:
  digitalWrite(step1,LOW);
  digitalWrite(step2,HIGH);
  digitalWrite(step3,LOW);
  digitalWrite(step4,LOW);
  break;
  case 2:
  digitalWrite(step1,LOW);
  digitalWrite(step2,LOW);
  digitalWrite(step3,HIGH);
  digitalWrite(step4,LOW);
  break;
  case 3:
  digitalWrite(step1,LOW);
  digitalWrite(step2,LOW);
  digitalWrite(step3,LOW);
  digitalWrite(step4,HIGH);
  break;
}
}else{
  switch(step_number){
  case 0:
  digitalWrite(step1,LOW);
  digitalWrite(step2,LOW);
  digitalWrite(step3,LOW);
  digitalWrite(step4,HIGH);
  break;
  case 1:
  digitalWrite(step1,LOW);
  digitalWrite(step2,LOW);
  digitalWrite(step3,HIGH);
  digitalWrite(step4,LOW);
  break;
  case 2:
  digitalWrite(step1,LOW);
  digitalWrite(step2,HIGH);
  digitalWrite(step3,LOW);
  digitalWrite(step4,LOW);
  break;
  case 3:
  digitalWrite(step1,HIGH);
  digitalWrite(step2,LOW);
  digitalWrite(step3,LOW);
  digitalWrite(step4,LOW);
  break; 
  } 
}
step_number++;
if(step_number > 3){
  step_number = 0;
}


}
