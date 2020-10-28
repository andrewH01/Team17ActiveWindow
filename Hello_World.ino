#include <IRremote.h>
int redLED = 13;
int button = 8;
int IRpin = 11;
int toggle = LOW;
bool mode = 0;
int rate = 100;
int cycle = 0;
IRrecv IR(IRpin);
decode_results cmd;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(redLED,OUTPUT);
pinMode(button,INPUT);
IR.enableIRIn();
}

void loop() {
  // put your main code here, to run repeatedly:
/*
while(IR.decode(&cmd) == 0){
}
Serial.println(cmd.value,HEX);
delay(1500);
IR.resume();
*/  
int val = digitalRead(button);
int previous = val;
while(previous == val and IR.decode(&cmd) == 0){
  previous = val;
  val = digitalRead(button);
  Switch(redLED,toggle,mode);
  
  }


Serial.println(cmd.value,HEX);

if (IR.decode(&cmd) == 0){
  if(previous == LOW and val == HIGH){
    toggle = not toggle;
    mode = 0;
  }
} 
else{
  if (cmd.value == 0xFF6897){
    toggle = HIGH;
    mode = 0;
    Serial.println("ON!");
  }
  else if (cmd.value == 0xFF30CF){
    toggle = LOW;
    mode = 0;
    Serial.println("OFF!");
  }
  else if (cmd.value == 0xFF7A85){
    mode = 1;
    Serial.println("Blink!");
  }
  else{
    toggle = not toggle;
    mode = 0;
    Serial.println("You're far away");
  }
}
delay(100);
IR.resume();
}

void Switch(int location, int tog,int mode,int cycle){
  if(mode == 0){
    digitalWrite(location,tog);
  }
  else if(mode == 1){
    digitalWrite(location,tog);
    delay(100);
    tog = not tog;
    digitalWrite(location,tog);
    delay(100); 
  }
}
