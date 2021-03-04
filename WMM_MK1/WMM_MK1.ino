
#include <SPI.h>             
#include <mcp2515.h>          
#include <Wire.h> 
int limswitch = 10;
int step1 = 7;
int step2 = 6;
int step3 = 5;
int step4 = 4; 
int step_number = 0;
int x = -1;
int recieved = 1;
int tracker = 3;
int calibrate = 0;
int moveLinches;
int moveRinches;
int moveLsteps;
int moveRsteps;
int i = 0;
int j = 0;
bool closer = true;
bool opener = false;
bool cal = true;
struct can_frame canMsg; 
MCP2515 mcp2515(10);


void setup() {
  pinMode(step1,OUTPUT);
  pinMode(step2,OUTPUT);
  pinMode(step3,OUTPUT);
  pinMode(step4,OUTPUT);  
  pinMode(limswitch, INPUT);
  SPI.begin();                      
  Serial.begin(9600);                


  /*
  mcp2515.reset();      
  mcp2515.setBitrate(CAN_500KBPS,MCP_8MHZ);  
  mcp2515.setNormalMode();                  
  canMsg.can_id  = 0x069;          
  canMsg.can_dlc = 1;              

  */

}



void loop() {
/*
//RECIEVING DESIRED POSITION FROM WPCM
  recieveMessage(0x036,1);
*/


//CALIBRATION CODE 
 while(recieved == calibrate)  {
  Serial.println("calibrating");
  while(digitalRead(limswitch)== 0 && cal){
    Serial.println();
    Serial.println();
    OneStep(cal);
    }
    cal = false; 
    Serial.println("Done Calibration");
    calibrate++;
 }

// MOVE WINDOW LEFT/OPEN
  if(recieved > tracker)  {
    Serial.println("opening");
    moveLinches = recieved - tracker;
    Serial.println(moveLinches);
    moveLsteps = moveLinches*750;
    Serial.println( moveLsteps );
    MOVELeft(i);
    tracker = recieved;
    Serial.println("tracker is");
    Serial.println(tracker);
    Serial.println("recieved is");
    Serial.println(recieved);
  }

//MOVE WINDOW RIGHT/CLOSE
  if(recieved < tracker)  {
    Serial.println("closing");
    moveRinches = tracker - recieved;
    Serial.println(moveLinches);
    moveRsteps = moveRinches*750;
    Serial.println( moveRsteps );
    MOVERight(j);
    tracker = recieved;
    
    Serial.println("tracker is");
    Serial.println(tracker);
    Serial.println("recieved is");
    Serial.println(recieved);
  }

/*
//SENDING UPDATED POSITION DATA TO WPCM  
    sendMessage(tracker);*/
   
}

//ONESTEP
void OneStep(bool ShadowWalker) {
 
  if(ShadowWalker){
  switch(step_number){
    case 0:
    digitalWrite(step1,HIGH);
    digitalWrite(step2,LOW);
    digitalWrite(step3,LOW);
    digitalWrite(step4,LOW);
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
    digitalWrite(step1,LOW);
    digitalWrite(step2,LOW);
    digitalWrite(step3,LOW);
    digitalWrite(step4,HIGH);
    break;
   
    } //SWITCH
  } //IF

  else{
    switch(step_number){
    case 0:
    digitalWrite(step1,LOW);
    digitalWrite(step2,LOW);
    digitalWrite(step3,LOW);
    digitalWrite(step4,HIGH);
 
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
    digitalWrite(step1,HIGH);
    digitalWrite(step2,LOW);
    digitalWrite(step3,LOW);
    digitalWrite(step4,LOW);
    break;
      }
    }
    step_number++;
    if(step_number > 3){
      step_number = 0;}
}//ONESTEP

//MOVELEFT
void MOVELeft(int b){
    while(b<moveLsteps){
      b++;
      Serial.println(b);
      OneStep(opener);
      delay(2);
      }
    
}//MOVELEFT

//MOVERIGHT
void MOVERight(int c){
    while(c<moveRsteps){
      c++;
      Serial.println(c);
      OneStep(closer);
      delay(2);
      }
     
}//MOVERIGHT




/*
//CANBUS CODE PACKAGE FROM LEAD WARRIOR MAXWELL RAPIER 
void sendMessage(int x){
  canMsg.data[0] = x;        
  mcp2515.sendMessage(&canMsg);     
}//SEND MESSAGE

void recieveMessage(int address, int bytes) {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    if(canMsg.can_id==address)  {
     x = canMsg.data[bytes-1];
     recieved = x;         
    }
    else  {
      x = -1;
    }  
  }     
 else {
  x = -1; 
 }
}//RECIEVE MESSAGE*/
