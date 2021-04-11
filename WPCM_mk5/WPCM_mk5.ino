#include <SPI.h>
#include <mcp2515.h>
  
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

// Colors used for testing, will be removed in later versions
#define RED     0xF800

/* Page value to currently display
 *  page = 0     :     Home
 *  page = 1     :     Active
 *  page = 2     :     Manual
 *  page = 3     :     Settings (WIP)
 *  page = 4     :     Schedule
 */
int next_page = 0;
int prev_page = next_page;
//====================================================
//====================DECLARES========================
//====================================================
struct can_frame canMsg1;
struct can_frame canMsg2;
struct can_frame canMsg3;
struct can_frame canMsg4;
struct can_frame canMsg;

int address = 100;
int bytes = 1;
int message = 45;

int OneEarlierA;
int TwoEarlierA;

int OneEarlierB;
int TwoEarlierB;

int OneEarlierC;
int TwoEarlierC;

int x1;
int x2;
int x3;


int receiveError;
int receiveTemp;
int auto_position;

int Desired = 2;


int Mode = 2;

unsigned long Current = millis();

MCP2515 chipA(10);
MCP2515 chipD(9);
MCP2515 chipE(8);


//====================================================
//====================SETUP===========================
//====================================================
void setup() {
  
  tft.begin();
  tft.setRotation(3);
  
  // Prepare the sliders screen
  drawHome();
  
  while (!Serial);
  Serial.begin(9600);
  
  
  chipA.reset();
  chipA.setBitrate(CAN_500KBPS);
  chipA.setNormalMode();

  chipD.reset();
  chipD.setBitrate(CAN_500KBPS);
  chipD.setNormalMode();

  chipE.reset();
  chipE.setBitrate(CAN_500KBPS);
  chipE.setNormalMode();


  canMsg1.can_id  = address;
  canMsg1.can_dlc = bytes;

  canMsg2.can_id = 69;
  canMsg2.can_dlc = bytes;

  canMsg3.can_id = 71;
  canMsg3.can_dlc = bytes;

  canMsg4.can_id = 72;
  canMsg4.can_dlc = bytes;
  
  canMsg.can_id = 101;
  canMsg.can_dlc = bytes;
  
  Serial.println("Example: Write to CAN");
}
//====================================================
//====================LOOP============================
//====================================================
void loop() {
  


 //-----------------TRANSMIT STUFF-------------------- 


Serial.print("Desired is: ");
Serial.println(Desired);

Serial.print("Mode is ");
Serial.println(Mode);

if(Mode == 2){
Serial.print("Desired is: ");
Serial.println(Desired);  
TransmitMessage(chipA, canMsg1, address, bytes, Desired);
}
if(Mode == 1){
Serial.print("Auto position is: ");
Serial.println(auto_position);
TransmitMessage(chipA,canMsg1,address,bytes,auto_position);  
}
if(Mode == 3){
Serial.print("recalibration mode ");
TransmitMessage(chipA,canMsg1,address,bytes,Desired); 

}

TransmitMessage(chipE,canMsg3,71,bytes,receiveTemp);



//------------------RECIEVE STUFF---------------------



ReceiveMessageA(chipA, canMsg, 101);
Serial.print("Error is ");
Serial.println(receiveError);


ReceiveMessageB(chipD, canMsg2, 69);


//Serial.print("Can msg id is ");
//Serial.println(canMsg2.can_id);
Serial.print("Temp is ");
Serial.println(receiveTemp);
//Serial.print("X2 is ");
//Serial.println(x2);

ReceiveMessageC(chipE,canMsg4,72);
Serial.print("Auto_position is ");
Serial.println(auto_position);



    if(next_page == 2){
      adjustSlider();
    }

    if(next_page == 0){

      // Update temperature to whatever you want,                                                                   LOOK HERE
      // possibly every minute or 30 seconds?
      
      drawTemperature();
    }
    
    // Retrieve a point  
    TSPoint p = ts.getPoint();
    
    // IF THERE IS NO USER INPUT, RE-LOOP
    if (p.z < MINPRESSURE || p.z > MAXPRESSURE) {
     return;
    }
    
    // Scale from ~0->4000 to tft.width using the calibration #'s
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    p.y = map(p.y, TS_MAXY, TS_MINY, -15, tft.height()+120);

// USED FOR TOUCH SCREEN vs. PIXEL DEBUGGING - X AND Y ARE SWAPPED DUE TO ROTATION
//    Serial.print("                X: ");
//    Serial.print(y);
//    Serial.print(", Y: ");
//    Serial.println(x);

    switch(next_page){
        case 0: loopHome(p.y,p.x);       break;
        case 1: loopActive(p.y,p.x);   break; 
        case 2: loopManual(p.y,p.x);     break;
        case 3: loopSettings(p.y,p.x);   break;
        case 4: loopSchedule(p.y,p.x);     break;
        default: Serial.println("Error with page value.");
    }

    if(next_page != prev_page){
      prev_page = next_page;
      switch(next_page){
        case 0: drawHome();       break;
        case 1: drawActive();     break; 
        case 2: drawManual();     break;
        case 3: drawSettings();   break;
        case 4: drawSchedule();   break;
        default: Serial.println("Error with page value.");
      }
    }


//----------------------------------------------------
}

//=====================================================
//=====================METHODS=========================
//=====================================================


void TransmitMessage(MCP2515 Device, struct can_frame Msg,int ID, int dlc, int data){

//Serial.print("Transmit Address is: ");
//Serial.println(Msg.can_id);
Msg.data[0] = data;
//Serial.print("Transmit Data is :");
//Serial.println(Msg.data[0]);
Device.sendMessage(&Msg);
//Serial.println("Message Transmitted");

  
}


void ReceiveMessageA(MCP2515 Device, struct can_frame Msg,int address){
  //Serial.print("Address is ");
  //Serial.println(address);
  //Serial.print("receive entered");
  /*Serial.println(Device.readMessage(&Msg));
  Serial.println(); 
  Serial.println(MCP2515::ERROR_OK); 
  Serial.println(); */
  
  if (Device.readMessage(&Msg) == MCP2515::ERROR_OK) {
    Serial.println("ENTERED R MESSAGE");
    //Serial.print("Message id is ");
    //Serial.println(Msg.can_id);
    //Serial.print("Message ");
  if (Msg.can_id == address){
    
    //Serial.print("I Made it into the conditional ");
    /*Serial.print(" The CAN_ID is ");
    Serial.print(Msg.can_id, DEC); // print ID
    Serial.print(" The CAN_dlc is ");
    Serial.print(" ");
    Serial.print(Msg.can_dlc, DEC); // print DLC
    Serial.print(" ");*/
   
    for (int i = 0; i<Msg.can_dlc; i++)  {  // print the data
      //Serial.print(" x1 is ");
      //Serial.println(Msg.data[i],DEC);
       x1 = Msg.data[i];
      if(x1 == OneEarlierA && x1 == TwoEarlierA)  {
      receiveError = x1;    
    }
    /*Serial.print("Before One Earlier is: ");
    Serial.println(OneEarlierA);
    Serial.print("Before Two Earlier is: ");
    Serial.println(TwoEarlierA);*/
    TwoEarlierA = OneEarlierA;
    OneEarlierA = x1;
    /*Serial.print("One Earlier is: ");
    Serial.println(OneEarlierA);
    Serial.print("Two Earlier is: ");
    Serial.println(TwoEarlierA);*/
    //Serial.println();
    //Serial.println("Messages recieved");      
  }
}
  }
  }

void ReceiveMessageB(MCP2515 Device, struct can_frame Msg,int address){
  //Serial.print("Address is ");
  //Serial.println(address);
  //Serial.print("receive entered");
  /*Serial.println(Device.readMessage(&Msg));
  Serial.println(); 
  Serial.println(MCP2515::ERROR_OK); 
  Serial.println(); */
  
  if (Device.readMessage(&Msg) == MCP2515::ERROR_OK) {
    Serial.println("ENTERED R MESSAGE");
    //Serial.print("Message id is ");
    //Serial.println(Msg.can_id);
    //Serial.print("Message ");
  if (Msg.can_id == address){
    
    //Serial.print("I Made it into the conditional ");
    Serial.print(" The CAN_ID is ");
    Serial.print(Msg.can_id, DEC); // print ID
    Serial.print(" The CAN_dlc is ");
    Serial.print(" ");
    Serial.print(Msg.can_dlc, DEC); // print DLC
    Serial.print(" ");
   
    for (int i = 0; i<Msg.can_dlc; i++)  {  // print the data
      Serial.print(" x2 is ");
      Serial.println(Msg.data[i],DEC);
       x2 = Msg.data[i];
      if(x2 == OneEarlierB && x2 == TwoEarlierB)  {
      receiveTemp = x2;    
    }
    /*Serial.print("Before One Earlier is: ");
    Serial.println(OneEarlierB);
    Serial.print("Before Two Earlier is: ");
    Serial.println(TwoEarlierB);*/
    TwoEarlierB = OneEarlierB;
    OneEarlierB = x2;
    /*Serial.print("One Earlier is: ");
    Serial.println(OneEarlierB);
    Serial.print("Two Earlier is: ");
    Serial.println(TwoEarlierB);*/
    //Serial.println();
    //Serial.println("Messages recieved");      
  }
}
  }
  }


void ReceiveMessageC(MCP2515 Device, struct can_frame Msg,int address){
  //Serial.print("Address is ");
  //Serial.println(address);
  //Serial.print("receive entered");
  /*Serial.println(Device.readMessage(&Msg));
  Serial.println(); 
  Serial.println(MCP2515::ERROR_OK); 
  Serial.println(); */
  
  if (Device.readMessage(&Msg) == MCP2515::ERROR_OK) {
    //Serial.println("ENTERED R MESSAGE");
    //Serial.print("Message id is ");
    //Serial.println(Msg.can_id);
    //Serial.print("Message ");
  if (Msg.can_id == address){
    
    //Serial.print("I Made it into the conditional ");
    /*Serial.print(" The CAN_ID is ");
    Serial.print(Msg.can_id, DEC); // print ID
    Serial.print(" The CAN_dlc is ");
    Serial.print(" ");
    Serial.print(Msg.can_dlc, DEC); // print DLC
    Serial.print(" ");*/
   
    for (int i = 0; i<Msg.can_dlc; i++)  {  // print the data
      //Serial.print(" x3 is ");
      //Serial.println(Msg.data[i],DEC);
       x3 = Msg.data[i];
      if(x3 == OneEarlierC && x3 == TwoEarlierC)  {
      auto_position = x3;    
    }
    /*Serial.print("Before One Earlier is: ");
    Serial.println(OneEarlierC);
    Serial.print("Before Two Earlier is: ");
    Serial.println(TwoEarlierC);*/
    TwoEarlierC = OneEarlierC;
    OneEarlierC = x3;
    /*Serial.print("One Earlier is: ");
    Serial.println(OneEarlierC);
    Serial.print("Two Earlier is: ");
    Serial.println(TwoEarlierC);*/
    //Serial.println();
    //Serial.println("Messages recieved");      
  }
}
  }
  }
  
//
// HOME FUNCTIONS
//

#define BUTTON_WIDTH 150     // Y height of button
#define BUTTON_HEIGHT 60     // X height of button

int temperature = 54;                 // The temperature in Fahrenheit
char tempbuff[20];                    // Temperature string buffer
char weather[20] = "Partly Cloudy";   // Weather text to display on screen (cloudy, sunny, raining)

void loopHome(int x, int y){
  
    // If touch is within button press threshold
    if (y > 375 && y < 450){
      
      // ACTIVE PRESSED
      if (x > 18 && x < 178) {       //Hard-coded location of slider touch values
          Mode = 1;
          next_page = 1;
      } 
      
      // MANUAL PRESSED
      if (x > 268 && x < 425) {         //Hard-coded location of slider touch values
          Mode = 2;
          next_page = 2;
      }
    }
    
    //SETTINGS PRESSED
    else if (x > 368 && y > 42 && x < 425 && y < 144){
        next_page = 3;
    }
}

void drawHome(){
    tft.fillScreen(DARKGRAY);
    drawTemperature();
    drawWeather();
    drawButtons();
    drawControl();
}

void drawTemperature(){

    // Format temperature for single digits
    if(temperature < 10){
      
      // Write temperature to middle of screen
      tft.setTextSize(7);
      tft.setTextColor(WHITE);
      tft.setCursor(185, 110);
      sprintf(tempbuff, "%d F", temperature);
      tft.print(tempbuff);

      // Print degree symbol
      tft.setTextSize(2);
      tft.setCursor(230, 105);
      tft.print("o");
    }

    // Format temperature for double digits
    if(temperature >= 10 && temperature < 100){
      
      // Write temperature to middle of screen
      tft.setTextSize(7);
      tft.setTextColor(WHITE);
      tft.setCursor(160, 110);
      sprintf(tempbuff, "%d F", temperature);
      tft.print(tempbuff);

      // Print degree symbol
      tft.setTextSize(2);
      tft.setCursor(245, 105);
      tft.print("o");
    }

    // Format temperature for triple digits
    if(temperature >= 100){
      
      // Write temperature to middle of screen
      tft.setTextSize(7);
      tft.setTextColor(WHITE);
      tft.setCursor(140, 110);
      sprintf(tempbuff, "%d F", temperature);
      tft.print(tempbuff);

      // Print degree symbol
      tft.setTextSize(2);
      tft.setCursor(265, 105);
      tft.print("o");
    }
}

void drawWeather(){

    // Write temperature to middle of screen
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.setCursor(163, 185);  // Will need to adjust X value based on number of letters
    tft.print(weather);
}

void drawButtons(){
    // Create home screen buttons
    tft.fillRoundRect(20,240,BUTTON_WIDTH,BUTTON_HEIGHT,8,GRAY);
    tft.fillRoundRect(152,240,BUTTON_WIDTH,BUTTON_HEIGHT,8,GRAY);
    tft.fillRoundRect(285,240,BUTTON_WIDTH,BUTTON_HEIGHT,8,GRAY);

    //Write text to buttons
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.setCursor(43, 260);
    tft.print("Active");
    tft.setCursor((2*BUTTON_WIDTH)-30, 260);
    tft.print("Manual");

    // Draw settings gear
    tft.fillCircle(428,50,26,GRAY);
    tft.fillCircle(428,50,10,DARKGRAY);
    tft.fillCircle(428,78,8,GRAY);       // Same X, Y+r
    tft.fillCircle(452,64,8,GRAY);       // X+rootX, Y+rootY
    tft.fillCircle(452,36,8,GRAY);       // X+rootX, Y-rootY
    tft.fillCircle(428,22,8,GRAY);       // Same X, Y-r
    tft.fillCircle(404,36,8,GRAY);       // X-rootX, Y-rootY
    tft.fillCircle(404,64,8,GRAY);       // X-rootX, Y+rootY
}

void drawControl(){
  // Use this function to draw the notice in the top left, stating what type of control the window is under

  // Write "Active Window MK2" in upper left corner of screen
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.setCursor(20, 20);  // 20 pixels from the left, 20 pixels from the top
    tft.print("Active Window MK2");
}


//
// ACTIVE MODE FUNCTIONS
//

void loopActive(int x, int y){
    if(x > 13 && x < 127 && y > 45 && y < 98){
      next_page = 0;
    }

    if(x > 188 && x < 355 && y > 288 && y < 355){
      ActiveButton();
    }
}

void drawActive(){
    tft.fillScreen(DARKGRAY);

    // Draw Home button
    tft.fillRoundRect(15,15,130,50,12,GRAY);
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.setCursor(45, 30);
    tft.print("Home");

    // Draw Home button
    tft.fillRoundRect(200,180,180,50,12,GRAY);
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.setCursor(230, 195);
    tft.print("Button");
}

void ActiveButton(){

Mode = 1;

}

//
// MANUAL MODE FUNCTIONS
//

#define BAR_Y         130               // Top y-value of slider
#define BAR_MINX      40               // X-axis of where slider bar begin
#define BAR_WIDTHX    40               // Width of slider button X direction
#define BAR_WIDTHY    40               // Width of slider button Y direction
#define SLIDE_WIDTHX  400              // Width of full slider bar Y direction
#define SLIDE_WIDTHY  BAR_WIDTHY+4     // Height of full slider bar X direction

#define SLIDE_MAXX    BAR_MINX+SLIDE_WIDTHX-BAR_WIDTHX-3    // Highest X position slider can reach
#define SLIDE_MINX    BAR_MINX+1                            // Lowest X position slider can reach
#define THRESH        10                                    // Threshold of X value that will snap slider to min/max

// Percent value of slider
int val = 0;

// Character buffers used to print slider percentages
char percent_buff[60];

// Pixel position of each slider
int desired = SLIDE_MINX, actual = SLIDE_MINX;

void loopManual(int x, int y){
  
    if (x > SLIDE_MINX && x < SLIDE_MAXX)
      {
        if(x < SLIDE_MINX + THRESH){ x = SLIDE_MINX; }    // Snap to minimum if within threshold
        if(x > SLIDE_MAXX - THRESH){ x = SLIDE_MAXX; }    // Snap to maximum if within threshold
        
        //SLIDER - center at Y=150
        if (y > 220-(BAR_WIDTHY/2) && y < 260+(BAR_WIDTHY/2)) {       //HARD-CODED location of slider touch values
            updateSlider(x);
        }
    }

    if(x > 13 && x < 127 && y > 45 && y < 98){
      next_page = 0;
    }
}

void updateSlider(int x) {
  
    // Erase the old bar value and redraw the new one
    drawDesired(desired, BAR_Y, DARKGRAY, false);
    desired = x;
    
    drawDesired(desired, BAR_Y, RED, true);

    // Update the stored value of the slider
    val = map(desired, SLIDE_MINX, SLIDE_MAXX, 0, 100);
    Desired = val + 2;
    sprintf(percent_buff, "%d%%", val);
  
    // Write the percentage slider value under the slider bar
    tft.fillRect(220, 190, 150, 50, DARKGRAY);
    tft.setTextSize(4);
    tft.setTextColor(WHITE);
    tft.setCursor(220, 190);
    tft.print(percent_buff);
}

void adjustSlider(){
    // Change the red slider value to match the user's input
    if(desired != actual){

      // Clear previous bottom triangle, -1 and +1 functions are used to solve zebra bug
      drawActual(actual-1, BAR_Y, DARKGRAY, false);
      drawActual(actual, BAR_Y, DARKGRAY, false);
      drawActual(actual+1, BAR_Y, DARKGRAY, false);
      
      if(desired > actual) { actual = actual+1; }
      else                 { actual = actual-1; }
    
      drawActual(actual, BAR_Y, RED, true);
    }
}

void drawManual() {

    // Make the screen blank, orient from portait to landscape mode
    tft.fillScreen(DARKGRAY);

    // Draw Home button
    tft.fillRoundRect(15,15,130,50,12,GRAY);
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.setCursor(45, 30);
    tft.print("Home");

    // Create black slider box outline
    tft.drawRect(SLIDE_MINX-2, BAR_Y, SLIDE_WIDTHX, SLIDE_WIDTHY, GRAY);
    tft.drawRect(SLIDE_MINX-3, BAR_Y-1, SLIDE_WIDTHX+2, SLIDE_WIDTHY+2, GRAY);

    // Draw initial slider button
    drawDesired(desired, BAR_Y, RED, true);
    drawActual(actual, BAR_Y, RED, true);

    // Update the stored value of the slider
    val = map(desired, SLIDE_MINX, SLIDE_MAXX, 0, 100);
    sprintf(percent_buff, "%d%%", val);

    // Write the percentage slider value under the slider bar
    tft.fillRect(220, 190, 150, 50, DARKGRAY);
    tft.setTextSize(4);
    tft.setTextColor(WHITE);
    tft.setCursor(220, 190);
    tft.print(percent_buff);
}

void drawDesired(int barPos_X, int barPos_Y, uint16_t color, boolean outline){
  
  int16_t x0, x1, x2, y0, y1, y2;

  // Coordinates for user selected location
  x0 = barPos_X;
  y0 = barPos_Y+BAR_WIDTHY;
  x1 = x0;
  y2 = barPos_Y+(2*BAR_WIDTHY/5);
  y1 = y2 + (BAR_WIDTHY/2)-7;
  y0 = y0 - BAR_WIDTHY+2;
  x2 = x0+(BAR_WIDTHX/2);
  y2 = y0;
  tft.fillTriangle(x0, y0, x1, y1, x2, y2, color);

  if(outline){
    tft.drawTriangle(x0, y0, x1, y1, x2, y2, BLACK);
  }

  x1 = x1 + BAR_WIDTHX-1;
  x0 = x1;
  tft.fillTriangle(x0, y0, x1, y1, x2, y2, color);

  if(outline){
    tft.drawTriangle(x0, y0, x1, y1, x2, y2, BLACK);
  }
}

void drawActual(int barPos_X, int barPos_Y, uint16_t color, boolean outline){

  int16_t x0, x1, x2, y0, y1, y2;
  
  // Coordinates for bottom triangle
  x0 = barPos_X;
  y0 = barPos_Y+BAR_WIDTHY;
  x1 = x0+BAR_WIDTHX-1;
  y1 = barPos_Y+BAR_WIDTHY;
  x2 = x0+(BAR_WIDTHX/2);
  y2 = barPos_Y+(2*BAR_WIDTHY/5);
  tft.fillTriangle(x0, y0, x1, y1, x2, y2, color);

  if(outline){
    tft.drawTriangle(x0, y0, x1, y1, x2, y2, BLACK);
  }
}


//
// SETTINGS FUNCTIONS
//

void loopSettings(int x, int y){
    if(x > 13 && x < 127 && y > 45 && y < 98){
      next_page = 0;
    }

    if(x > 220 && x < 420 && y > 365 && y < 430){
      recalibrate();
    }
}

void drawSettings(){
    tft.fillScreen(DARKGRAY);

    // Draw Home button
    tft.fillRoundRect(15,15,130,50,12,GRAY);
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.setCursor(45, 30);
    tft.print("Home");

    // Draw Lock button
    tft.fillRoundRect(235,55,220,60,12,GRAY);
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.setCursor(248, 75);
    tft.print("Lock");
    drawUnlocked();                           // Need to add an if statement here to draw switch based on a boolean variable

    // Draw Units button
    tft.fillRoundRect(235,140,220,60,12,GRAY);
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.setCursor(248, 160);
    tft.print("Units");
    drawMetric();                             // Need to add an if statement here to draw switch based on a boolean variable
    
    // Draw Recalibrate button
    tft.fillRoundRect(235,225,220,60,12,GRAY);
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.setCursor(248, 245);
    tft.print("Recalibrate");
}

void drawUnlocked(){
    // Draw switch in unlocked position
    tft.fillRect(370,65,50,41,DARKGRAY);
    tft.fillCircle(370,85,20,DARKGRAY);
    tft.fillCircle(420,85,20,DARKGRAY);
    tft.fillCircle(370,85,16,GRAY);
}

void drawLocked(){
    // Draw switch in locked position
    tft.fillRect(370,65,50,41,DARKGRAY);
    tft.fillCircle(370,85,20,DARKGRAY);
    tft.fillCircle(420,85,20,DARKGRAY);
    tft.fillCircle(420,85,16,GRAY);
}

void drawMetric(){
    // Draw switch in metric position
    tft.fillRect(370,150,50,41,DARKGRAY);
    tft.fillCircle(370,170,20,DARKGRAY);
    tft.fillCircle(420,170,20,DARKGRAY);
    tft.fillCircle(370,170,16,GRAY);
}

void drawImperial(){
    // Draw switch in imperial position
    tft.fillRect(370,150,50,41,DARKGRAY);
    tft.fillCircle(370,170,20,DARKGRAY);
    tft.fillCircle(420,170,20,DARKGRAY);
    tft.fillCircle(420,170,16,GRAY);
}

void recalibrate(){
Mode = 3;
Desired = 1;
}

//
//  SCHEDULE FUNCTIONS
//

int schedRadioButton = 0;   // 0 = Open, 1 = Close, 2 = Delete
boolean morningEvent = false;
boolean afternoonEvent = false;
boolean eveningEvent = false;
boolean openInMorning = false;
boolean openInAfternoon = false;
boolean openInEvening = false;

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

   // If Home button is pressed
   if(x > 13 && x < 127 && y > 45 && y < 98){
     next_page = 0;
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
