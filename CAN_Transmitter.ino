#include <SPI.h>          //Library for using SPI Communication 
#include <mcp2515.h>      //Library for using CAN Communication

#define potPin A0   
int potValue=0;
//int tempValue=0;    

struct can_frame canMsg;
MCP2515 mcp2515(10); // chip select CS pin 10

void setup() 
{
  Serial.begin(9600);
  SPI.begin();               //Begins SPI communication
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS,MCP_8MHZ); //Sets CAN at speed 500KBPS and Clock 8MHz
  mcp2515.setNormalMode();

  canMsg.can_id  = 0x036;           //CAN id as 0x036
  canMsg.can_dlc = 1;               //CAN data length as 1 byte
  
 
}

void loop() 
{
  potValue = analogRead(potPin);
  potValue = map(potValue,0,1023,0,255);
  Serial.println(potValue);
    
  canMsg.data[0] = potValue;        //Update pot value in [0]
  //canMsg.data[1]= 0x00;
  mcp2515.sendMessage(&canMsg);     //Sends the CAN message
  Serial.println(mcp2515.readMessage(&canMsg));
  //delay(200);
}
