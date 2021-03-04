#define AN A0   
#include <Wire.h>                           
#include "SparkFunBME280.h" 

float humidity;
float temperature;
float pressure;
int lux;
unsigned long time;

BME280 capture;                                 // refer to BME280 object as capture
void setup(void){
Serial.begin(9600);                             
Wire.begin();                                   // initialize i2c

//Configuration 
capture.settings.I2CAddress = 0x76;             // I2C address of the BME280 when SD0 = 0 
capture.settings.runMode = 3;                   // Sleep mode between measurements 
capture.settings.tStandby = 0;                  // Standby between 2 measurements is 0.5ms  
capture.settings.filter = 0;                    // Filtering Inactive
capture.settings.tempOverSample = 1;            // 16bit temperature resolution
capture.settings.humidOverSample = 1;           // 16bit humidity resolution
capture.settings.pressOverSample = 1;           // 16bit pressure resolution

delay(10);     
capture.begin();
Serial.print("Temperature (C)");
Serial.print(",");
Serial.print("Humidity (%)");
Serial.print(",");
Serial.print("Pressure (hPa)");
Serial.print(",");
Serial.print("Illuminance (lux)");
Serial.print(",");
Serial.println("Time (ms)");
}

void loop(){
  temperature = capture.readTempC();              // get temp data
  humidity = capture.readFloatHumidity();         // get humidity data
  pressure = capture.readFloatPressure()/100;     // get pressure data
  lux = analogRead(AN);
  time=millis();
  
  Serial.print(temperature);
  Serial.print(",");
  Serial.print(humidity);
  Serial.print(",");
  Serial.print(pressure);
  Serial.print(",");
  Serial.print(lux);
  Serial.print(",");
  Serial.print(time);
  Serial.print("\n");
  delay(3600000);                                 // delay between samples
}
