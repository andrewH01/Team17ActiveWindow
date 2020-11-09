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

#define REDBAR_Y      80               // Top y-value of red slider
#define GREENBAR_Y    130              // Top y-value of green slider
#define BLUEBAR_Y     180              // Top y-value of blue slider
#define BAR_MINX      50               // X-axis of where slider bars begin
#define BAR_WIDTHX    25               // Width of slider button X direction
#define BAR_WIDTHY    25               // Width of slider button Y direction
#define SLIDE_WIDTHX  250              // Width of full slider bar Y direction
#define SLIDE_WIDTHY  BAR_WIDTHY+4     // Height of full slider bar X direction

#define SLIDE_MAXX    BAR_MINX+SLIDE_WIDTHX-BAR_WIDTHX-3    // Highest X position slider can reach
#define SLIDE_MINX    BAR_MINX+1                            // Lowest X position slider can reach
#define THRESH        10                                    // Threshold of X value that will snap slider to min/max

// Percent value of each slider
int redval = 0;
int greenval = 0;
int blueval = 0;

// Character buffers used to print slider percentages
char redbuff[60];
char greenbuff[60];
char bluebuff[60];

// Pixel position of each slider
int redDesired = SLIDE_MINX, redActual = SLIDE_MINX;
int greenDesired = SLIDE_MINX, greenActual = SLIDE_MINX;
int blueDesired = SLIDE_MINX, blueActual = SLIDE_MINX;

// Scaled x and y coordinate values of touch screen to display
int x, y;

void setup() {
  
  //Necessary code to start up the touch screen
  Serial.begin(9600);
  //Serial.println("Serial Monitor Test begin");

  // Set backlight to digital pin 3 and set HIGH
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);

  // Begin the touch screen
  tft.begin();

  // If there's issue with starting the touch screen, do not proceed further
  if (!ts.begin()) {
    while (1);
  }
  
  tft.setRotation(1);

  // Prepare the sliders screen
  drawSliders();
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
    if(redDesired != redActual){

      // Clear previous bottom triangle, -1 and +1 functions are used to solve zebra bug
      drawActual(redActual-1, REDBAR_Y, LIGHTCYAN, false);
      drawActual(redActual, REDBAR_Y, LIGHTCYAN, false);
      drawActual(redActual+1, REDBAR_Y, LIGHTCYAN, false);
      
      if(redDesired > redActual) { redActual = redActual+1; }
      else                       { redActual = redActual-1; }
    
      drawActual(redActual, REDBAR_Y, RED, true);
    }

    // Change the green slider value to match the user's input
    if(greenDesired != greenActual){

      // Clear previous bottom triangle, -1 and +1 functions are used to solve zebra bug
      drawActual(greenActual-1, GREENBAR_Y, LIGHTCYAN, false);
      drawActual(greenActual, GREENBAR_Y, LIGHTCYAN, false);
      drawActual(greenActual+1, GREENBAR_Y, LIGHTCYAN, false);
      
      if(greenDesired > greenActual) { greenActual = greenActual+1; }
      else                           { greenActual = greenActual-1; }
    
      drawActual(greenActual, GREENBAR_Y, GREEN, true);
    }

    // Change the blue slider value to match the user's input
    if(blueDesired != blueActual){

      // Clear previous bottom triangle, -1 and +1 functions are used to solve zebra bug
      drawActual(blueActual-1, BLUEBAR_Y, LIGHTCYAN, false);
      drawActual(blueActual, BLUEBAR_Y, LIGHTCYAN, false);
      drawActual(blueActual+1, BLUEBAR_Y, LIGHTCYAN, false);
      
      if(blueDesired > blueActual) { blueActual = blueActual+1; }
      else                         { blueActual = blueActual-1; }
    
      drawActual(blueActual, BLUEBAR_Y, BLUE, true);
    }


    
    // IF THERE IS NO USER INPUT, RE-LOOP
    if (! ts.touched()) {
      return;
    }
    
    // Retrieve a point  
    TS_Point p = ts.getPoint();
  
    // Scale from ~0->4000 to tft.width using the calibration #'s
    x = map(p.y, TS_MINY, TS_MAXY,  0, DISP_MAXX-15); //-10 seems good for the 2.8" TFT Touch Shield
    y = map(p.x, TS_MINX, TS_MAXX, 0, DISP_MAXY); //DISP_MAXX

    // Debugging in serial monitor to determine touch location vs pixel location
    Serial.print("P.X: ");
    Serial.print(p.x);
    Serial.print(", P.Y: ");
    Serial.print(p.y);
    
    Serial.print("                X: ");
    Serial.print(x);
    Serial.print(", Y: ");
    Serial.println(y);
    
    if (x > SLIDE_MINX && x < SLIDE_MAXX)
    {
      if(x < SLIDE_MINX + THRESH){ x = SLIDE_MINX; }    // Snap to minimum if within threshold
      if(x > SLIDE_MAXX - THRESH){ x = SLIDE_MAXX; }    // Snap to maximum if within threshold
      
      //RED SLIDER - center at Y=150
      if (y > 150-(BAR_WIDTHY/2) && y < 150+(BAR_WIDTHY/2)) {       //Hard-coded location of slider touch values
          updateRedSlider();
      } 
      
      // GREEN SLIDER - center at Y=90
      if (y > 90-(BAR_WIDTHY/2) && y < 90+(BAR_WIDTHY/2)) {         //Hard-coded location of slider touch values
          updateGreenSlider();
      }
  
      // BLUE SLIDER - center at Y=35
      if (y > 35-(BAR_WIDTHY/2) && y < 35+(BAR_WIDTHY/2)) {         //Hard-coded location of slider touch values
          updateBlueSlider();
      }
    }
}


void updateRedSlider() {
  
    // Erase the old bar value and redraw the new one
    drawDesired(redDesired, REDBAR_Y, LIGHTCYAN, false);
    redDesired = x;
    drawDesired(redDesired, REDBAR_Y, RED, true);

  
    // Update the stored value of the slider
    redval = map(redDesired, SLIDE_MINX, SLIDE_MAXX, 0, 100);
    sprintf(redbuff, "%d%%", redval);
  
    // Write the percentage slider value to the left of the slider bar
    tft.fillRect(20, 86, 23, 15, LIGHTCYAN);
    tft.setTextSize(1);
    tft.setTextColor(GREY);
    tft.setCursor(20, 86);
    tft.print(redbuff);
}


void updateGreenSlider(){
  
    // Erase the old bar value and redraw the new one
    drawDesired(greenDesired, GREENBAR_Y, LIGHTCYAN, false);
    greenDesired = x;
    drawDesired(greenDesired, GREENBAR_Y, GREEN, true);
    
  
    // Update the stored value of the slider
    greenval = map(greenDesired, SLIDE_MINX, SLIDE_MAXX, 0, 100);
    sprintf(greenbuff, "%d%%", greenval);
  
    // Write the percentage slider value to the left of the slider bar
    tft.fillRect(20, 134, 23, 15, LIGHTCYAN);
    tft.setTextSize(1);
    tft.setTextColor(GREY);
    tft.setCursor(20, 134);
    tft.print(greenbuff);
}


void updateBlueSlider() {
  
    // Erase the old bar value and redraw the new one
    drawDesired(blueDesired, BLUEBAR_Y, LIGHTCYAN, false);
    blueDesired = x;
    drawDesired(blueDesired, BLUEBAR_Y, BLUE, true);

  
    // Update the stored value of the slider
    blueval = map(blueDesired, SLIDE_MINX, SLIDE_MAXX, 0, 100);
    sprintf(bluebuff, "%d%%", blueval);
  
    // Write the percentage slider value to the left of the slider bar
    tft.fillRect(20, 182, 23, 15, LIGHTCYAN);
    tft.setTextSize(1);
    tft.setTextColor(GREY);
    tft.setCursor(20, 182);
    tft.print(bluebuff);
}


/*  -- Do not touch -- 
 *  Draws sliders on the screen based on position {COLOR}BAR_Y with length SLIDE_WIDTHX at SLIDE_MINX-2
 *  Draw the initial slider buttons at their lowest position. 
 */
void drawSliders() {

    // Make the screen blank, orient from portait to landscape mode
    tft.fillScreen(LIGHTCYAN);

    // Write "Active Window MK1" in upper left corner of screen
    tft.setTextSize(2);
    tft.setTextColor(GREY);
    tft.setCursor(90, 28);  // 20 pixels from the left, 20 pixels from the top
    tft.print("Active Window MK3");

    // Draw back arrow
    tft.fillRoundRect(20,20,40,30,8,RED);
    tft.fillTriangle(28,35,36,25,36,45,WHITE);
    tft.fillRect(36,30,12,10,WHITE);

    // Create black slider box outlines
    tft.drawRect(SLIDE_MINX-2, REDBAR_Y, SLIDE_WIDTHX, SLIDE_WIDTHY, GREY);
    tft.drawRect(SLIDE_MINX-2, GREENBAR_Y, SLIDE_WIDTHX, SLIDE_WIDTHY, GREY);
    tft.drawRect(SLIDE_MINX-2, BLUEBAR_Y, SLIDE_WIDTHX, SLIDE_WIDTHY, GREY);

    // Draw initial slider buttons
    drawDesired(redDesired, REDBAR_Y, RED, true);
    drawDesired(greenDesired, GREENBAR_Y, GREEN, true);
    drawDesired(blueDesired, BLUEBAR_Y, BLUE, true);
    drawActual(redActual, REDBAR_Y, RED, true);
    drawActual(greenActual, GREENBAR_Y, GREEN, true);
    drawActual(blueActual, BLUEBAR_Y, BLUE, true);

    // Write initial percentages of each slider
    tft.setTextSize(1);
    tft.setTextColor(GREY);
    tft.setCursor(20, 86);
    tft.print("0%");
    tft.setCursor(20, 134);
    tft.print("0%");
    tft.setCursor(20, 182);
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
