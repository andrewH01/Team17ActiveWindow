//USE ME
#include <SPI.h>             
#include <mcp2515.h>          
#include <Wire.h> 
int limswitch = 3;
int step1 = 7;
int step2 = 6;
int step3 = 5;
int step4 = 4; 
int step_number = 0;
int x = -1;
int motoruse;
int tracker;
int calibrate = 1;
int moveLinches;
int moveRinches;
int moveLsteps;
int moveRsteps;
int i = 0;
int j = 0;
int Final;
int OneEarlier;
int TwoEarlier;
bool closer = true;
bool opener = false;
bool cal = true;
struct can_frame canMsg; 
MCP2515 chipB(10);  
int address = 100;
int bussend;
struct can_frame canMsg1;
int Error = 0; 




void setup() {
  pinMode(step1,OUTPUT);
  pinMode(step2,OUTPUT);
  pinMode(step3,OUTPUT);
  pinMode(step4,OUTPUT);  
  SPI.begin();                                                         
  canMsg.can_id  = 100;          
  canMsg.can_dlc = 1;
  while (!Serial);
  Serial.begin(9600);
  chipB.reset();
  chipB.setBitrate(CAN_500KBPS);
  chipB.setNormalMode();  
  canMsg1.can_id = 101;
  canMsg1.can_dlc = 1;        
}

void loop() {

//RECIEVING DESIRED POSITION FROM WPCM
  
  //Serial.println(canMsg.can_id);
  ReceiveMessage(chipB, canMsg, address);
 
  if(bussend != motoruse){
    motoruse = bussend;
  }
  //Serial.print(" motoruse ");
  //Serial.println(motoruse);
  Serial.print(" bussend ");
  Serial.println(bussend);
  
//Serial.println(canMsg.data[0]);
    
//CALIBRATION CODE 
 while(motoruse == calibrate &&  motoruse != 0)  {
  Serial.println("calibrating");
  while(digitalRead(limswitch)== 0 && cal){
    Serial.println();
    Serial.println();
    OneStep(cal);
    }
    cal = false; 
    Serial.println(digitalRead(limswitch));
    Serial.println("Done Calibration");
    tracker = motoruse;
    calibrate = 0;
 }

// MOVE WINDOW LEFT/OPEN
  if(motoruse > tracker && motoruse > 0)  {
    Serial.println("OPEN");
    Serial.println("motoruse IS");
    Serial.println(motoruse);
    moveLinches = motoruse - tracker;
    moveLsteps = moveLinches*50;
    MOVELeft(i);
    tracker = motoruse;
    Serial.println("tracker is");
    Serial.println(tracker);
    Serial.println("motoruse is");
    Serial.println(motoruse);
  }

//MOVE WINDOW RIGHT/CLOSE
  if(motoruse < tracker && motoruse > 0)  {
    Serial.println("CLOSE");
    Serial.println("motoruse IS");
    Serial.println(motoruse);
    moveRinches = tracker - motoruse;
    moveRsteps = moveRinches*50;
    MOVERight(j);
    tracker = motoruse;
    Serial.println("tracker is");
    Serial.println(tracker);
    Serial.println("motoruse is");
    Serial.println(motoruse);
  }

//SENDING UPDATED POSITION DATA TO WPCM  
    
    TransmitMessage(chipB, canMsg1, 101, 1, Error);
   
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
     
}

//MOVERIGHT

//CANBUS CODE PACKAGE FROM LEAD WARRIOR MAXWELL RAPIER 


//SEND MESSAGE   

void TransmitMessage(MCP2515 Device, struct can_frame Msg,int ID, int dlc, int data){

Msg.data[0] = data;
Device.sendMessage(&Msg);
Serial.println("Message Transmitted");

}

//SEND MESSAGE


void ReceiveMessage(MCP2515 Device, struct can_frame Msg,int address){
  /*Serial.println(Device.readMessage(&Msg));
  Serial.println(); 
  Serial.println(MCP2515::ERROR_OK); 
  Serial.println(); */
  if (Device.readMessage(&Msg) == MCP2515::ERROR_OK) {
    //Serial.println("ENTERED R MESSAGE");
  if (Msg.can_id == address){
   
    /*Serial.print("I Made it into the conditional ");
    Serial.print(" The CAN_ID is ");
    Serial.print(Msg.can_id, DEC); // print ID
    Serial.print(" The CAN_dlc is ");
    Serial.print(" ");
    Serial.print(Msg.can_dlc, DEC); // print DLC
    Serial.print(" ");*/
   
    for (int i = 0; i<Msg.can_dlc; i++)  {  // print the data
      Serial.print(" The CAN_data is ");
      Serial.print(Msg.data[i],DEC);
      x = Msg.data[i];
      if(x == OneEarlier && x == TwoEarlier)  {
      bussend = x;    
    }
    TwoEarlier = OneEarlier;
    OneEarlier = x;
    //Serial.println();
    //Serial.println("Messages recieved");      
  }
}
  }
  }
