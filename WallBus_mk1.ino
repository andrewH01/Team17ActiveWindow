#include <SPI.h>
#include <mcp2515.h>
//====================================================
//====================DECLARES========================
//====================================================
struct can_frame canMsg1;
struct can_frame canMsg;

int address = 69;
int bytes = 1;
int message = 3;

int pos;

int OneEarlierA;
int TwoEarlierA;

int x1;

int temp;


MCP2515 chipF(10);


void setup() {
  // put your setup code here, to run once:
  canMsg1.can_id  = address;
  canMsg1.can_dlc = bytes;


  canMsg.can_id = 99;
  canMsg.can_dlc = bytes;

  while (!Serial);
  Serial.begin(9600);

  chipF.reset();
  chipF.setBitrate(CAN_500KBPS);
  chipF.setNormalMode();

}

void loop() {
  // put your main code here, to run repeatedly:


  ReceiveMessage(chipF,canMsg,69,OneEarlierA,TwoEarlierA,temp,x1);
  


if (temp < 51){

  pos = 1;
  
}

else{

 pos = 100;
  
}
  
  
  
  TransmitMessage(chipF, canMsg1,69,bytes,pos);

    
}


//=====================================================
//=====================METHODS=========================
//=====================================================


void TransmitMessage(MCP2515 Device, struct can_frame Msg,int ID, int dlc, int data){

Serial.print("Transmit entered");
Msg.data[0] = data;
Device.sendMessage(&Msg);
//Serial.println("Message Transmitted");

  
}


void ReceiveMessage(MCP2515 Device, struct can_frame Msg,int address, int OneEarlier, int TwoEarlier,int receive,int x){
  Serial.print("receive entered");
  /*Serial.println(Device.readMessage(&Msg));
  Serial.println(); 
  Serial.println(MCP2515::ERROR_OK); 
  Serial.println(); */
  
  if (Device.readMessage(&Msg) == MCP2515::ERROR_OK) {
    //Serial.println("ENTERED R MESSAGE");
    Serial.print("Message id is ");
    Serial.println(Msg.can_id);
    Serial.print("Message ");
  if (Msg.can_id == address){
    
    Serial.print("I Made it into the conditional ");
    Serial.print(" The CAN_ID is ");
    Serial.print(Msg.can_id, DEC); // print ID
    Serial.print(" The CAN_dlc is ");
    Serial.print(" ");
    Serial.print(Msg.can_dlc, DEC); // print DLC
    Serial.print(" ");
   
    for (int i = 0; i<Msg.can_dlc; i++)  {  // print the data
      Serial.print(" The CAN_data is ");
      Serial.println(Msg.data[i],DEC);
       x = Msg.data[i];
      if(x == OneEarlier && x == TwoEarlier)  {
      receive = x;    
    }
    TwoEarlier = OneEarlier;
    OneEarlier = x;
    //Serial.println();
    Serial.println("Messages recieved");      
  }
}
  }
  }
