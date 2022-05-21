
 

#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
  #include <avr/power.h>
#endif


#define PIN_LED1       2 
#define PIN_INPUT       0 


#define NUMPIXELS      132
bool reset = 0;
int led = 0;
Adafruit_NeoPixel pixels_1 = Adafruit_NeoPixel(NUMPIXELS, PIN_LED1, NEO_GRB + NEO_KHZ800);

int incomingByte = 0; // for incoming serial data
 

void setup() {

  pixels_1.setBrightness(25u);
  Serial.begin(9600);
  pixels_1.begin(); // This initializes the NeoPixel library.
  pinMode(PIN_INPUT,INPUT);
  pixels_1.setPixelColor(led, pixels_1.Color(0,255,0));

}
void loop() {

  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    
    if(incomingByte == 'u'){ // up
      pixels_1.setPixelColor(led, pixels_1.Color(0,0,0));
      led++;
      pixels_1.setPixelColor(led, pixels_1.Color(0,255,0));
    }
    else if(incomingByte == 'd'){ // down
      pixels_1.setPixelColor(led, pixels_1.Color(0,0,0));
      led--;
      pixels_1.setPixelColor(led, pixels_1.Color(0,255,0));
    }
    Serial.println(led);
  }
  // if(digitalRead(PIN_INPUT) == 1 && reset == 0){
  //   reset = 1;
  //   if(led == 0){
  //     pixels_1.setPixelColor(led, pixels_1.Color(0,255,0));
  //     led++; 
  //   }
  //   else{
  //     pixels_1.setPixelColor(led, pixels_1.Color(0,0,0));
  //     led++;
  //     pixels_1.setPixelColor(led, pixels_1.Color(0,255,0));
  //   }
  // }
  // if(digitalRead(PIN_INPUT) == 0){ 
  //   reset = 0;
  // } 
  pixels_1.show();
}











