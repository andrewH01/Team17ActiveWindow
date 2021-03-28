/*  Andrew Hartnett
 *  11/1/2020
 *  Active Window PCM MK2
 *  
 *  This program uses the Arduino TFT 2.8" Touch Screen Display to implement our
 *  Active Window Primary Control Module GUI. The program takes from the Adafruit_ILI9341
 *  touchpaint example.
 *  
 *  The program will serve as the menu for the display. It will show the current temperature
 *  in Fahrenheit as well as the current weather. Menu buttons are displayed to allow the user
 *  to switch between manual control of the window through sliders and add/remove a schedule.
 *  
 *  References
 *  touchpaint: https://github.com/adafruit/Adafruit_ILI9341/blob/master/examples/touchpaint/touchpaint.ino
 *  
 *  List of To-Dos:
 *  - Add button functionality
 *  - Set up auto-center functions for changing text length like temp and weather
 *  - Touch up menu display, look to Nest thermostat for ideas
 */

#include <Adafruit_GFX.h>       // Core graphics library
#include <SPI.h>                // Needed for communication w/ board
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
#define LIGHTCYAN   tft.color565(233, 237, 247) 
#define WHITE       0xFFFF

// Colors used for testing, will be removed in later versions
#define RED     0xF800

#define BUTTON_WIDTH 175     // Y height of button
#define BUTTON_HEIGHT 60     // X height of button

int temperature = 54;                 // The temperature in Fahrenheit
char tempbuff[20];                    // Temperature string buffer
char weather[20] = "Partly Cloudy";   // Weather text to display on screen (cloudy, sunny, raining)

int x, y;

void setup() {
  //Necessary code to start up the touch screen
    Serial.begin(9600);
  
    // Begin the touch screen
    tft.begin();
    tft.setRotation(3);
  
    // Prepare the sliders screen
    drawMenu();
    
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

    x = p.y;
    y = p.x;

// USED FOR TOUCH SCREEN vs. PIXEL DEBUGGING
    Serial.print("                X: ");
    Serial.print(x);
    Serial.print(", Y: ");
    Serial.println(y);
  
    // If touch is within button press threshold
    if (y > 375 && y < 450){
      
      // SCHEDULE PRESSED
      if (x > 18 && x < 178) {       //Hard-coded location of slider touch values
          Serial.println("Schedule pressed!");
      } 
      
      // MANUAL PRESSED
      if (x > 268 && x < 425) {         //Hard-coded location of slider touch values
          Serial.println("Manual pressed!");
      }
    }
}

void drawMenu(){
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
      tft.setCursor(105, 70);
      sprintf(tempbuff, "%d F", temperature);
      tft.print(tempbuff);

      // Print degree symbol
      tft.setTextSize(2);
      tft.setCursor(150, 65);
      tft.print("o");
    }

    // Format temperature for double digits
    if(temperature >= 10 && temperature < 100){
      
      // Write temperature to middle of screen
      tft.setTextSize(7);
      tft.setTextColor(GRAY);
      tft.setCursor(80, 70);
      sprintf(tempbuff, "%d F", temperature);
      tft.print(tempbuff);

      // Print degree symbol
      tft.setTextSize(2);
      tft.setCursor(165, 65);
      tft.print("o");
    }

    // Format temperature for triple digits
    if(temperature >= 100){
      
      // Write temperature to middle of screen
      tft.setTextSize(7);
      tft.setTextColor(WHITE);
      tft.setCursor(60, 70);
      sprintf(tempbuff, "%d F", temperature);
      tft.print(tempbuff);

      // Print degree symbol
      tft.setTextSize(2);
      tft.setCursor(185, 65);
      tft.print("o");
    }
}

void drawWeather(){

    // Write temperature to middle of screen
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.setCursor(80, 135);  // Will need to adjust X value based on number of letters
    tft.print(weather);
}

void drawButtons(){
    // Create black slider box outlines
    tft.fillRoundRect(20,240,BUTTON_WIDTH,BUTTON_HEIGHT,8,GRAY);
    tft.fillRoundRect(285,240,BUTTON_WIDTH,BUTTON_HEIGHT,8,GRAY);

    //Write text to buttons
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.setCursor(35, 260);
    tft.print("Schedule");
    tft.setCursor((2*BUTTON_WIDTH)-30, 260);
    tft.print("Manual");
}

void drawControl(){
  // Use this function to draw the notice in the top left, stating what type of control the window is under

  // Write "Active Window MK2" in upper left corner of screen
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.setCursor(20, 20);  // 20 pixels from the left, 20 pixels from the top
    tft.print("Active Window MK2");
}
