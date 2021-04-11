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



int main (void){
  ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));    
  ADMUX |= (1<<REFS0);
  ADMUX &= ~(1<<REFS1);             
  ADCSRB &= ~((1<<ADTS2)|(1<<ADTS1)|(1<<ADTS0));    
  ADCSRA |= (1<<ADATE);                
  ADCSRA |= (1<<ADEN);               
  ADCSRA |= (1<<ADSC);              
  while (!Serial);
  Serial.begin(9600);
  chipC.reset();
  chipC.setBitrate(CAN_500KBPS);
  chipC.setNormalMode();  
  canMsg1.can_id = 69;
  canMsg1.can_dlc = 1;
  DDRC = 

/////////////////////////////////////////////////////////////////////
while(1){
sensorInput = = ADCW;        
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
}
void TransmitMessage(MCP2515 Device, struct can_frame Msg,int ID, int dlc, int data){

Msg.data[0] = data;
Device.sendMessage(&Msg);
Serial.println("Message Transmitted");

}

//SEND MESSAGE
