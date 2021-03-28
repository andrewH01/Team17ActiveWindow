#include <SPI.h>             
#include <mcp2515.h>          
#include <Wire.h>
int x = -1;
int Final;
int OneEarlier;
int TwoEarlier;
struct can_frame canMsg; 
MCP2515 chipC(10);  
int address = 100;
int bussend;
struct can_frame canMsg1;
int Error = 0;
int sensePin = A0;  
int sensorInput;    
double temp;        



void setup() {
  while (!Serial);
  Serial.begin(9600);
  
  chipC.reset();
  chipC.setBitrate(CAN_500KBPS);
  chipC.setNormalMode();  
  canMsg1.can_id = 69;
  canMsg1.can_dlc = 1;
}
/////////////////////////////////////////////////////////////////////
void loop() {
sensorInput = analogRead(A0);        
  temp = (double)sensorInput / 1024;   
  temp = temp * 5;                     
  temp = temp - 0.5;                   
  temp = temp * 100;
  temp = temp * 9/5 + 32;
 Serial.print(" Current Temperature: ");
 int IntValue = (int) temp;
 Serial.println(IntValue);
 TransmitMessage(chipC, canMsg1, 69, 1, IntValue);

 
}///////////////////////////////////////////////////////////////////////LOOP

void TransmitMessage(MCP2515 Device, struct can_frame Msg,int ID, int dlc, int data){

Msg.data[0] = data;
Device.sendMessage(&Msg);
Serial.println("Message Transmitted");

}

//SEND MESSAGE
