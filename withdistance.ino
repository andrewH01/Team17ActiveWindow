int LED = 6;
int limswitch = 13;
int direct = 12;
int step1 = 11;
int step2 = 10;
int step3 = 9;
int step4 = 8;
int step_number = 0;
int state = 0;
int buttonPushCounter = 0;
int buttonState = 0;         
int lastButtonState = 0;

bool input = false;
int pressure = HIGH;


void setup() {
  
  pinMode(LED, OUTPUT);
  pinMode(step1,OUTPUT);
  pinMode(step2,OUTPUT);
  pinMode(step3,OUTPUT);
  pinMode(step4,OUTPUT);
  pinMode(limswitch, INPUT);
  pinMode(direct, INPUT);
  Serial.begin(9600);
}


void loop() {
  
//MOTOR CODE 
  
  DirectionCheck(pressure);
  delay(2);
  state = digitalRead(limswitch);  
  if(state == HIGH)  {
    OneStep(input);
    delay(2);}
 
  else  {
    delay(2);
  }

}//VOID LOOP

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
  
      }//SWITCH
    }//ELSE
  
    step_number++;
    if(step_number > 3){
      step_number = 0;}
 
}//ONESTEP


void DirectionCheck(int pushnumber) { 
  pushnumber = digitalRead(direct);
   
  if (pushnumber != lastButtonState) {

    if (pushnumber == HIGH) {
      buttonPushCounter++;
      }
    
    else {}
    
    }
  
  lastButtonState = pushnumber;

  if (buttonPushCounter % 2 == 1) {
    delay(2);
    input = !input;
    //Serial.println(input);
    digitalWrite(LED, HIGH);
    }
     
   else {
    digitalWrite(LED, LOW);
    Serial.println(input);  
    }

}//DirectionCheck
