/*  Andrew Hartnett
 *  10/23/2020
 *  Active Window PCM MK1
 *  
 *  This program uses the Arduino TFT 2.8" Touch Screen Display to implement our
 *  Active Window Primary Control Module GUI. The program takes from the Adafruit_ILI9341
 *  touchpaint example, as well as an RGB-Mixer program written by Joseph Justin.
 *  
 *  The program consists of 3 horizontal sliders that the user can interact with.
 *  Each slider represents a different aspect of the Active Window (the window itself,
 *  the thermal cover, and shade heights). To the right of each slider, the % value is
 *  written. When the user decides a new slider value, instead of instantly changing to
 *  the new value, the slider will update its value in steps towards the desired value.
 *  This will work as a homing mechanism to allow for adjustments to the slider value
 *  after a previous adjustment has already been made.
 *  
 *  References
 *  touchpaint: https://github.com/adafruit/Adafruit_ILI9341/blob/master/examples/touchpaint/touchpaint.ino
 *  RGB-Mixer (found within larger program): http://www.notespoint.com/wp-content/uploads/2017/08/tft_lcd_sketch.txt
 *  RGB-Mixer full program page: http://www.notespoint.com/tftlcdshield-arduino-menu/
 *  
 *  List of To-Dos:
 *  - Fix backlight delay implementation
 *  - Add symbols for window, thermal cover, blinds, shades
 *  - Seperate each slider into 2 markers, one for current position and one for desired position
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

// Scaled x and y coordinate values of touch screen to display
int x, y;

void setup() {
  
  //Necessary code to start up the touch screen
  Serial.begin(9600);
  //Serial.println("Serial Monitor Test begin");

  tft.begin();
  tft.setRotation(3);

  // Prepare the sliders screen
  drawSlider();
}

void loop() {
    /* 
     If no one use touching the screen, work on homing mechanism
     Will need:
     
     - Rise/fall speed variable
     - Current height of window, thermal cover, and blinds
     - Desired height of window, thermal cover, and blinds
     - Angle of blinds
     - Maybe something for curtains
  
     For now, this if statement will be left blank
     */
    
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

    // Debugging in serial monitor to determine touch location vs pixel location
    Serial.print("X: ");
    Serial.println(x);
    Serial.print("Y: ");
    Serial.println(y);
    Serial.println();
    
    if (x > SLIDE_MINX && x < SLIDE_MAXX)
    {
      if(x < SLIDE_MINX + THRESH){ x = SLIDE_MINX; }    // Snap to minimum if within threshold
      if(x > SLIDE_MAXX - THRESH){ x = SLIDE_MAXX; }    // Snap to maximum if within threshold
      
      //SLIDER - center at Y=150
      if (y > 220-(BAR_WIDTHY/2) && y < 260+(BAR_WIDTHY/2)) {       //HARD-CODED location of slider touch values
          updateSlider();
      }
    }
}


void updateSlider() {
  
    // Erase the old bar value and redraw the new one
    drawDesired(desired, BAR_Y, DARKGRAY, false);
    desired = x;
    drawDesired(desired, BAR_Y, RED, true);

    // Update the stored value of the slider
    val = map(desired, SLIDE_MINX, SLIDE_MAXX, 0, 100);
    sprintf(percent_buff, "%d%%", val);
  
    // Write the percentage slider value to the left of the slider bar
    tft.fillRect(220, 190, 150, 50, DARKGRAY);
    tft.setTextSize(4);
    tft.setTextColor(WHITE);
    tft.setCursor(220, 190);
    tft.print(percent_buff);
}

/*  -- Do not touch -- 
 *  Draws slider on the screen based on position BAR_Y with length SLIDE_WIDTHX at SLIDE_MINX-2
 *  Draw the initial slider button at their lowest position.
 */
void drawSlider() {

    // Make the screen blank, orient from portait to landscape mode
    tft.fillScreen(DARKGRAY);

    // Write "Active Window MK1" in upper left corner of screen
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.setCursor(90, 28);  // 20 pixels from the left, 20 pixels from the top
    tft.print("Active Window MK3");

    // Draw back arrow
    tft.fillRoundRect(20,20,40,30,8,GRAY);
    tft.fillTriangle(28,35,36,25,36,45,WHITE);
    tft.fillRect(36,30,12,10,WHITE);

    // Create black slider box outline
    tft.drawRect(SLIDE_MINX-2, BAR_Y, SLIDE_WIDTHX, SLIDE_WIDTHY, GRAY);
    tft.drawRect(SLIDE_MINX-3, BAR_Y-1, SLIDE_WIDTHX+2, SLIDE_WIDTHY+2, GRAY);

    // Draw initial slider button
    drawDesired(desired, BAR_Y, RED, true);
    drawActual(actual, BAR_Y, RED, true);

    // Write initial percentages of each slider
    tft.setTextSize(4);
    tft.setTextColor(WHITE);
    tft.setCursor(220, 190);
    tft.print("0%");
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
