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
#include <Adafruit_ILI9341.h>   // This is one of the chips of the TFT shield
#include <Adafruit_STMPE610.h>  // This is another chip on the TFT shield

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 260
#define TS_MINY 280
#define TS_MAXX 3770
#define TS_MAXY 3775

// These are the length/width of the display
#define DISP_MAXX 320
#define DISP_MAXY 240

// The STMPE610 uses hardware SPI on the shield, and #8
#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);


// WIP **********************
// Amount of time before backlight shut offs when inactive
// When testing values for backlight delay
// 10000 - too quick
// 20000 - too quick
// 30000 - still too quick
// 40000 - doesn't go to sleep, will keep value here until fixable
#define BACKLIGHT_DELAY 40000


// Define some TFT readable colour codes to human readable names
#define BLACK       0x0000
#define GREY        tft.color565(64, 64, 64)
#define SILVER      tft.color565(137, 140, 145)
#define LIGHTCYAN   tft.color565(233, 237, 247) 
#define WHITE       0xFFFF

// Colors used for testing, will be removed in later versions
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F

#define BUTTON_WIDTH 100     // Y height of button
#define BUTTON_HEIGHT 32     // X height of button

int temperature = 54;                 // The temperature in Fahrenheit
char tempbuff[20];                    // Temperature string buffer
char weather[20] = "Partly Cloudy";   // Weather text to display on screen (cloudy, sunny, raining)

int x, y;

void setup() {
  
    //Necessary code to start up the touch screen
    Serial.begin(9600);
  
    // Set backlight to digital pin 3 and set HIGH
    pinMode(3, OUTPUT);
    digitalWrite(3, HIGH);
  
    // Begin the touch screen
    tft.begin();
  
    // If there's issue with starting the touch screen, do not proceed further
    if (!ts.begin()) {
      while (1);
    }
    
    // Make the screen blank, orient from portait to landscape mode
    tft.fillScreen(GREY);
    tft.setRotation(1);
  
    // Write "Active Window MK1" in upper left corner of screen
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.setCursor(20, 20);  // 20 pixels from the left, 20 pixels from the top
    tft.print("Active Window MK2");
  
    // Prepare the sliders screen
    drawMenu();
}

void loop() {
    
    if (! ts.touched()) {
      return;
    }

    // Retrieve a point  
    TS_Point p = ts.getPoint();
  
    // Scale from ~0->4000 to tft.width using the calibration #'s
    x = map(p.y, TS_MINY, TS_MAXY,  0, DISP_MAXX);
    y = map(p.x, TS_MINX, TS_MAXX, 0, DISP_MAXY);

// USED FOR TOUCH SCREEN vs. PIXEL DEBUGGING
//    Serial.print("P.X: ");
//    Serial.print(p.x);
//    Serial.print(", P.Y: ");
//    Serial.print(p.y);
//    
//    Serial.print("                X: ");
//    Serial.print(x);
//    Serial.print(", Y: ");
//    Serial.println(y);
  
    // If touch is within button press threshold
    if (y > 14 && y < 46){
      
      // MENU PRESSED
      if (x > 5 && x < 79) {       //Hard-coded location of slider touch values
          Serial.println("Menu pressed!");
      } 
      
      // CONTROL PRESSED
      if (x > 86 && x < 186) {         //Hard-coded location of slider touch values
          Serial.println("Control pressed!");
      }
  
      // SCHEDULE PRESSED
      if (x > 201 && x < 301) {         //Hard-coded location of slider touch values
          Serial.println("Schedule pressed!");
      }
    }
}

void drawMenu(){
    drawTemperature();
    drawWeather();
    drawButtons();
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
      tft.setTextColor(WHITE);
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
    tft.setTextColor(SILVER);
    tft.setCursor(80, 135);  // Will need to adjust X value based on number of letters
    tft.print(weather);
}

void drawButtons(){
  
    // Create black slider box outlines
    tft.fillRect(6, 190, BUTTON_WIDTH-20, BUTTON_HEIGHT, RED);
    tft.fillRect(BUTTON_WIDTH-6, 190, BUTTON_WIDTH, BUTTON_HEIGHT, GREEN);
    tft.fillRect(1+(2*BUTTON_WIDTH), 190, BUTTON_WIDTH+10, BUTTON_HEIGHT, BLUE);

    //Write text to buttons
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.setCursor(22, 199);
    tft.print("Menu");
    tft.setCursor(3+BUTTON_WIDTH, 199);
    tft.print("Control");
    tft.setCursor(9+(2*BUTTON_WIDTH), 199);
    tft.print("Schedule");
}
