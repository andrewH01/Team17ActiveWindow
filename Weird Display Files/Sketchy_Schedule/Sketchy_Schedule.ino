#include <Adafruit_GFX.h>       // Core graphics library
#include <Adafruit_HX8357.h>   // This is one of the chips of the TFT shield
#include "TouchScreen.h"

// These are the four touchscreen analog pins
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 5   // can be a digital pin
#define XP 6   // can be a digital pin

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 110
#define TS_MINY 80
#define TS_MAXX 900
#define TS_MAXY 940

#define MINPRESSURE 10
#define MAXPRESSURE 1000

// The display uses hardware SPI, plus #9 & #10
#define TFT_RST -1  // dont use a reset pin, tie to arduino RST if you like
#define TFT_DC 4
#define TFT_CS 7
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Define some TFT readable colour codes to human readable names
#define BLACK       0x0000
#define DARKGRAY    tft.color565(51, 51, 51)
#define GRAY        tft.color565(128, 140, 145)
#define LIGHTGRAY   tft.color565(175, 175, 175)
#define LIGHTCYAN   tft.color565(233, 237, 247) 
#define WHITE       0xFFFF

int schedRadioButton = 0;   // 0 = Open, 1 = Close, 2 = Delete
boolean morningEvent = false;
boolean afternoonEvent = false;
boolean eveningEvent = false;
boolean openInMorning = false;
boolean openInAfternoon = false;
boolean openInEvening = false;
int Desired = 2;

void setup() {
    tft.begin();
    tft.setRotation(3);
    
    // Prepare the sliders screen
    drawSchedule();
    
    while (!Serial);
    Serial.begin(9600);
}

void loop() {

   // Retrieve a point  
   TSPoint p = ts.getPoint();
   
   // IF THERE IS NO USER INPUT, RE-LOOP
   if (p.z < MINPRESSURE || p.z > MAXPRESSURE) {
     return;
   }
   
   // Scale from ~0->4000 to tft.width using the calibration #'s
   p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
   p.y = map(p.y, TS_MAXY, TS_MINY, -15, tft.height()+120);
   
   loopSchedule(p.y,p.x);
}

void drawSchedule(){
   
   // Make the screen blank, orient from portait to landscape mode
   tft.fillScreen(DARKGRAY);
  
   // Draw Home button
   tft.fillRoundRect(15,15,130,50,12,GRAY);
   tft.setTextSize(3);
   tft.setTextColor(BLACK);
   tft.setCursor(45, 30);
   tft.print("Home");

   // Time printed to screen
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.setCursor(200, 22);
   //sprintf(tempbuff, "%d F", temperature);
   tft.print("12:27 PM");

   // Fast forward button
   tft.fillRoundRect(365,20,85,45,12,GRAY);
   tft.fillTriangle(385, 27, 385, 57, 405, 42, DARKGRAY);
   tft.fillTriangle(405, 27, 405, 57, 425, 42, DARKGRAY);
   tft.fillRect(425, 27, 5, 30, DARKGRAY);

   // Draw Morning, Afternoon, and Night buttons
   tft.fillRoundRect(200,85,170,60,12,GRAY);
   tft.fillRoundRect(200,165,170,60,12,GRAY);
   tft.fillRoundRect(200,245,170,60,12,GRAY);
   tft.setTextColor(BLACK);
   tft.setCursor(222, 105);
   tft.print("Morning");
   tft.setCursor(206, 185);
   tft.print("Afternoon");
   tft.setCursor(222, 265);
   tft.print("Evening");

   // Draw Open and Close radio buttons
   tft.fillRoundRect(20,100,150,190,12,GRAY);
   tft.fillRoundRect(35,110,120,50,12,LIGHTGRAY);
   tft.fillRoundRect(35,170,120,50,12,LIGHTGRAY);
   tft.fillRoundRect(35,230,120,50,12,LIGHTGRAY);
   tft.setCursor(60, 125);
   tft.print("Open");
   tft.setCursor(52, 185);
   tft.print("Close");
   tft.setCursor(43, 245);
   tft.print("Delete");
   
   if(schedRadioButton == 0){
     selectOpen();
   }
   else if(schedRadioButton == 1){
     selectClose();
   }
   else{
     selectDelete();
   }
}

void loopSchedule(int x, int y){

   // USED FOR TOUCH SCREEN vs. PIXEL DEBUGGING - X AND Y ARE SWAPPED DUE TO ROTATION
   Serial.print("X: ");
   Serial.print(x);
   Serial.print(", Y: ");
   Serial.println(y);

   // If Home button is pressed
   if(x > 13 && x < 127 && y > 45 && y < 98){
     //next_page = 0;
   }

   // If Open/Close is pressed
   else if(x > 30 && x < 130){
     // For Open
     if(y > 167 && y < 232){
        if(schedRadioButton != 0){
          selectOpen();
        }
     }
     // For Close
     else if(y > 268 && y < 330){
        if(schedRadioButton != 1){
          selectClose();
        }
     }
     // For Delete
     else if(y > 363 && y < 424){
        if(schedRadioButton != 2){
          selectDelete();
        }
     }
   }

   else if(x > 182 && x < 340){
     
     // For Morning
     if(y > 133 && y < 220){
        scheduleMorning();
     }
     
     // For Afternoon
     if(y > 265 && y < 345){
        scheduleAfternoon();
     }
     
     // For Evening
     if(y > 390 && y < 450){
        scheduleEvening();
     }
   }

   if(x > 343 && x < 418 && y < 100 && y > 40){
        fastForward();
   }

   // Need to an if statement that checks every minute if the current                                       LOOK HERE
   // time is equal to an event time (Morning, Afternoon, Evening)
   
}

void selectOpen(){

   // Select Open
   tft.drawRoundRect(35,110,120,50,12,DARKGRAY);
   tft.drawRoundRect(36,111,118,48,12,DARKGRAY);

   // Deselect Close
   tft.drawRoundRect(35,170,120,50,12,LIGHTGRAY);
   tft.drawRoundRect(36,171,118,48,12,LIGHTGRAY);

   // Deselect Delete
   tft.drawRoundRect(35,230,120,50,12,LIGHTGRAY);
   tft.drawRoundRect(36,231,118,48,12,LIGHTGRAY);

   schedRadioButton = 0;
}

void selectClose(){

   // Deselect Open
   tft.drawRoundRect(35,110,120,50,12,LIGHTGRAY);
   tft.drawRoundRect(36,111,118,48,12,LIGHTGRAY);

   // Select Close
   tft.drawRoundRect(35,170,120,50,12,DARKGRAY);
   tft.drawRoundRect(36,171,118,48,12,DARKGRAY);

   // Deselect Delete
   tft.drawRoundRect(35,230,120,50,12,LIGHTGRAY);
   tft.drawRoundRect(36,231,118,48,12,LIGHTGRAY);

   schedRadioButton = 1;
}

void selectDelete(){

   // Deselect Open
   tft.drawRoundRect(35,110,120,50,12,LIGHTGRAY);
   tft.drawRoundRect(36,111,118,48,12,LIGHTGRAY);

   // Deselect Close
   tft.drawRoundRect(35,170,120,50,12,LIGHTGRAY);
   tft.drawRoundRect(36,171,118,48,12,LIGHTGRAY);

   // Select Delete
   tft.drawRoundRect(35,230,120,50,12,DARKGRAY);
   tft.drawRoundRect(36,231,118,48,12,DARKGRAY);

   schedRadioButton = 2;
}

void scheduleMorning(){
   if(schedRadioButton == 2){
       tft.fillRect(390,105, 70, 30, DARKGRAY);
       morningEvent = false;
       return;
   }
  
   // Schedule Open or Close in the Morning and draw event to screen
   tft.setTextColor(WHITE);
   tft.setTextSize(2);
   tft.setCursor(390, 105);
   
   if(schedRadioButton == 0){
      if(!openInMorning){
         tft.fillRect(390,105, 70, 30, DARKGRAY);
      }
      openInMorning = true;
      tft.print("OPEN");
   }
   else if(schedRadioButton == 1){
      if(openInMorning){
         tft.fillRect(390,105, 70, 30, DARKGRAY);
      }
      openInMorning = false;
      tft.print("CLOSE");
   }

   morningEvent = true;
}

void scheduleAfternoon(){
   if(schedRadioButton == 2){
      tft.fillRect(390,185, 70, 30, DARKGRAY);
      afternoonEvent = false;
      return;
   }
  
   // Schedule Open or Close in the Afternoon and draw event to screen
   tft.setTextColor(WHITE);
   tft.setTextSize(2);
   tft.setCursor(390, 185);
   
   if(schedRadioButton == 0){
      if(!openInAfternoon){
         tft.fillRect(390,185, 70, 30, DARKGRAY);
      }
      openInAfternoon = true;
      tft.print("OPEN");
   }
   else if(schedRadioButton == 1){
      if(openInAfternoon){
         tft.fillRect(390,185, 70, 30, DARKGRAY);
      }
      openInAfternoon = false;
      tft.print("CLOSE");
   }
   
   afternoonEvent = true;
}

void scheduleEvening(){
   if(schedRadioButton == 2){
       tft.fillRect(390,265, 70, 30, DARKGRAY);
       eveningEvent = false;
       return;
   }
  
   // Schedule Open or Close in the Evening and draw event to screen
   tft.setTextColor(WHITE);
   tft.setTextSize(2);
   tft.setCursor(390, 265);

   if(schedRadioButton == 0){
      if(!openInEvening){
         tft.fillRect(390,265, 70, 30, DARKGRAY);
      }
      openInEvening = true;
      tft.print("OPEN");
   }
   else if(schedRadioButton == 1){
      if(openInEvening){
         tft.fillRect(390,265, 70, 30, DARKGRAY);
      }
      openInEvening = false;
      tft.print("CLOSE");
   }

   eveningEvent = true;
}

void fastForward(){

   // Code to set current time to the next event time (Morning, Afternoon, Night)                                     LOOK HERE
   
}
