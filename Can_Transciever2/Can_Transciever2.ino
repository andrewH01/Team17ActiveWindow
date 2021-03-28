#include <SPI.h>              //Library for using SPI Communication 
#include <mcp2515.h>          //Library for using CAN Communication
#include <Wire.h> 
int step1 = 7;
int step2 = 6;
int step3 = 5;
int step4 = 4; 
int step_number = 0;
int button = 3;
int LED = 2;

int x = -1;

int recieved;

struct can_frame canMsg; 

MCP2515 mcp2515(10);                 // SPI CS Pin 10 
 
void setup() {

  pinMode(step1,OUTPUT);
  pinMode(step2,OUTPUT);
  pinMode(step3,OUTPUT);
  pinMode(step4,OUTPUT);
  pinMode(LED,OUTPUT);
  pinMode(button,INPUT);
    
  SPI.begin();                       //Begins SPI communication
  
  Serial.begin(9600);                //Begins Serial Communication at 9600 baudrate 
  
  mcp2515.reset();      
  mcp2515.setBitrate(CAN_500KBPS,MCP_8MHZ); //Sets CAN at speed 500KBPS and Clock 8MHz 
  mcp2515.setNormalMode();                  //Sets CAN at normal mode

  canMsg.can_id  = 0x069;           //CAN id as 0x036
  canMsg.can_dlc = 1;               //CAN data length as 1 byte
}

void loop() 
{
  recieveMessage(0x036,1);

  
  
  if(recieved < 128 && recieved != -1){
      Serial.print("Turning CW - Recieved Value ");
      Serial.println(recieved);
      OneStep(true);
      delay(recieved);
     }
  else if(recieved == -1){
  }
  else{
      Serial.print("Turning CCW - Recieved Value ");
      Serial.println(recieved);
      OneStep(false);
      delay(255 -recieved);
     }
 
   
  int capture = digitalRead(button);
  //Serial.println(capture);

  if (capture == HIGH){
    
    while (capture == HIGH){
      capture = digitalRead(button);
    }
    sendMessage(1);
  } 
}




void sendMessage(int x){
  canMsg.data[0] = x;        //Update pot value in [0]
  //canMsg.data[1]= 0x00;
  mcp2515.sendMessage(&canMsg);     //Sends the CAN message
}


void recieveMessage(int address, int bytes){
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) // To receive data (Poll Read)
  {
    if(canMsg.can_id==address)
    {
     x = canMsg.data[bytes-1];
     recieved = x;         
    }
    else{
      x = -1;
    }  
}     
 else{
  x = -1; 
 }
}

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
