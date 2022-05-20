

#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
  #include <avr/power.h>
#endif


#define PIN_LED1       2 


#define NUMPIXELS      132
#define BUOYS          60

Adafruit_NeoPixel pixels_1 = Adafruit_NeoPixel(NUMPIXELS, PIN_LED1, NEO_GRB + NEO_KHZ800);

 
typedef struct 
{
  int nr;
  int stripe;
  int nr_led;
  int color[3];
  int numOfSeq;
  int counter;
  uint32_t sequence[19];
  uint32_t timer;
} buoy;

typedef struct 
{
  int nr;
  int stripe;
  int nr_led;
  int color[3];
  int colorCounter;
  int dimmingState; //0 0ff / 1 up / -1 down
} lighthouse;


// #include "/home/dymitr/Documents/2022_01-Locja_super_yachts/firmware/esp32-wrover-b/buoy_leds/led_list.txt"
#include "/home/dymitr/Dokumenty/13-05-2022_Buoy-leds/buoy_leds/led_list.txt"

uint32_t lhTimer;
uint32_t now;

int lhdel = 10; //delay for all lighthouse

int led_on_Jastarnia = 0;
int led_on_Kaszyca = 0;
int led_on_Hel = 0;
int led_on_Gdansk = 0;
int led_on_Sopot = 0;
int led_on_Krynica = 0;
int led_on_Shchukinskiy = 0;
int led_on_Obzornyy = 0;
int led_on_Taran = 0;
int led_on_Rozewie = 0;

void setup() {

  pixels_1.setBrightness(255);

  pixels_1.begin(); // This initializes the NeoPixel library.


  for(int bid = 0; bid < BUOYS; bid++){
    buoyList[bid].timer = millis();
  }

  lhTimer = millis();

}
 
void setLed(int stripeNr, int ledNr, int R, int G, int B){
  pixels_1.setPixelColor(ledNr, pixels_1.Color(R,G,B));      
  // pixels_1.show();    
}
void loop() {
  
  //BUOYS
  now = millis();
  for(unsigned int a = 0; a < BUOYS; a = a + 1 ){
    if(buoyList[a].counter == 0){
      setLed(buoyList[a].stripe,buoyList[a].nr_led,buoyList[a].color[0],buoyList[a].color[1],buoyList[a].color[2]);
    }
    if(now - buoyList[a].timer >= buoyList[a].sequence[buoyList[a].counter]){
      if(buoyList[a].counter == buoyList[a].numOfSeq){
        buoyList[a].counter = 0;
      }
      else buoyList[a].counter += 1;
  
      if(buoyList[a].counter % 2 == 0){
        setLed(buoyList[a].stripe,buoyList[a].nr_led,buoyList[a].color[0],buoyList[a].color[1],buoyList[a].color[2]);
      }
      else{
        setLed(buoyList[a].stripe,buoyList[a].nr_led,0,0,0);
      }
      buoyList[a].timer = millis();
    }
  }
  pixels_1.show();  

  //LIGHTHOUSES
  now = millis();
  if(now - lhTimer >= lhdel){ 
  
  //HEL LIGHTHOUSE

    for(unsigned int a = 0; a < Hel_n_leds; a = a + 1 ){
      if(Hel[a].dimmingState == 1){
        if(Hel[a].colorCounter < Hel[a].color[0]){
          Hel[a].colorCounter+=1;
          pixels_1.setPixelColor(Hel[a].nr_led, pixels_1.Color(Hel[a].colorCounter,Hel[a].colorCounter,Hel[a].colorCounter/2));      
        }
        else if(Hel[a].colorCounter >= Hel[a].color[0]){
          Hel[a].dimmingState = -1;
        }
      }
      if(Hel[a].dimmingState == -1){
        if(Hel[a].colorCounter == Hel[a].color[0]/2){
          if(a<Hel_n_leds-1){
            led_on_Hel=a+1;
          }
          else if(a==Hel_n_leds-1){
            led_on_Hel=0;
          }
          Hel[led_on_Hel].dimmingState = 1;
        }
        if(Hel[a].colorCounter > 1){
          Hel[a].colorCounter-=1;  
          pixels_1.setPixelColor(Hel[a].nr_led, pixels_1.Color(Hel[a].colorCounter,Hel[a].colorCounter,Hel[a].colorCounter/2));         
        }
        else if(Hel[a].colorCounter <= 1){
          Hel[a].dimmingState = 0;
          pixels_1.setPixelColor(Hel[a].nr_led, pixels_1.Color(0,0,0));      
        }
      }
    }

    //JASTARNIA LIGHTHOUSE

    for(unsigned int a = 0; a < Jastarnia_n_leds; a = a + 1 ){
      if(Jastarnia[a].dimmingState == 1){
        if(Jastarnia[a].colorCounter < Jastarnia[a].color[0]){
          Jastarnia[a].colorCounter+=1;
          pixels_1.setPixelColor(Jastarnia[a].nr_led, pixels_1.Color(Jastarnia[a].colorCounter,Jastarnia[a].colorCounter,Jastarnia[a].colorCounter/2));      
        }
        else if(Jastarnia[a].colorCounter >= Jastarnia[a].color[0]){
          Jastarnia[a].dimmingState = -1;
        }
      }
      if(Jastarnia[a].dimmingState == -1){
        if(Jastarnia[a].colorCounter == Jastarnia[a].color[0]/2){
          if(a<Jastarnia_n_leds-1){
            led_on_Jastarnia=a+1;
          }
          else if(a==Jastarnia_n_leds-1){
            led_on_Jastarnia=0;
          }
          Jastarnia[led_on_Jastarnia].dimmingState = 1;
        }
        if(Jastarnia[a].colorCounter > 1){
          Jastarnia[a].colorCounter-=1;  
          pixels_1.setPixelColor(Jastarnia[a].nr_led, pixels_1.Color(Jastarnia[a].colorCounter,Jastarnia[a].colorCounter,Jastarnia[a].colorCounter/2));         
        }
        else if(Jastarnia[a].colorCounter <= 1){
          Jastarnia[a].dimmingState = 0;
          pixels_1.setPixelColor(Jastarnia[a].nr_led, pixels_1.Color(0,0,0));      
        }
      }
    }
    //KASZYCA LIGHTHOUSE

    for(unsigned int a = 0; a < Kaszyca_n_leds; a = a + 1 ){
      if(Kaszyca[a].dimmingState == 1){
        if(Kaszyca[a].colorCounter < Kaszyca[a].color[0]){
          Kaszyca[a].colorCounter+=1;
          pixels_1.setPixelColor(Kaszyca[a].nr_led, pixels_1.Color(Kaszyca[a].colorCounter,Kaszyca[a].colorCounter,Kaszyca[a].colorCounter/2));      
        }
        else if(Kaszyca[a].colorCounter >= Kaszyca[a].color[0]){
          Kaszyca[a].dimmingState = -1;
        }
      }
      if(Kaszyca[a].dimmingState == -1){
        if(Kaszyca[a].colorCounter == Kaszyca[a].color[0]/2){
          if(a<Kaszyca_n_leds-1){
            led_on_Kaszyca=a+1;
          }
          else if(a==Kaszyca_n_leds-1){
            led_on_Kaszyca=0;
          }
          Kaszyca[led_on_Kaszyca].dimmingState = 1;
        }
        if(Kaszyca[a].colorCounter > 1){
          Kaszyca[a].colorCounter-=1;  
          pixels_1.setPixelColor(Kaszyca[a].nr_led, pixels_1.Color(Kaszyca[a].colorCounter,Kaszyca[a].colorCounter,Kaszyca[a].colorCounter/2));         
        }
        else if(Kaszyca[a].colorCounter <= 1){
          Kaszyca[a].dimmingState = 0;
          pixels_1.setPixelColor(Kaszyca[a].nr_led, pixels_1.Color(0,0,0));      
        }
      }
    }
    //GDANSK LIGHTHOUSE

    for(unsigned int a = 0; a < Gdansk_n_leds; a = a + 1 ){
      if(Gdansk[a].dimmingState == 1){
        if(Gdansk[a].colorCounter < Gdansk[a].color[0]){
          Gdansk[a].colorCounter+=1;
          pixels_1.setPixelColor(Gdansk[a].nr_led, pixels_1.Color(Gdansk[a].colorCounter,Gdansk[a].colorCounter,Gdansk[a].colorCounter/2));      
        }
        else if(Gdansk[a].colorCounter >= Gdansk[a].color[0]){
          Gdansk[a].dimmingState = -1;
        }
      }
      if(Gdansk[a].dimmingState == -1){
        if(Gdansk[a].colorCounter == Gdansk[a].color[0]/2){
          if(a<Gdansk_n_leds-1){
            led_on_Gdansk=a+1;
          }
          else if(a==Gdansk_n_leds-1){
            led_on_Gdansk=0;
          }
          Gdansk[led_on_Gdansk].dimmingState = 1;
        }
        if(Gdansk[a].colorCounter > 1){
          Gdansk[a].colorCounter-=1;  
          pixels_1.setPixelColor(Gdansk[a].nr_led, pixels_1.Color(Gdansk[a].colorCounter,Gdansk[a].colorCounter,Gdansk[a].colorCounter/2));         
        }
        else if(Gdansk[a].colorCounter <= 1){
          Gdansk[a].dimmingState = 0;
          pixels_1.setPixelColor(Gdansk[a].nr_led, pixels_1.Color(0,0,0));      
        }
      }
    }
    //SOPOT LIGHTHOUSE

    for(unsigned int a = 0; a < Sopot_n_leds; a = a + 1 ){
      if(Sopot[a].dimmingState == 1){
        if(Sopot[a].colorCounter < Sopot[a].color[0]){
          Sopot[a].colorCounter+=1;
          pixels_1.setPixelColor(Sopot[a].nr_led, pixels_1.Color(Sopot[a].colorCounter,Sopot[a].colorCounter,Sopot[a].colorCounter/2));      
        }
        else if(Sopot[a].colorCounter >= Sopot[a].color[0]){
          Sopot[a].dimmingState = -1;
        }
      }
      if(Sopot[a].dimmingState == -1){
        if(Sopot[a].colorCounter == Sopot[a].color[0]/2){
          if(a<Sopot_n_leds-1){
            led_on_Sopot=a+1;
          }
          else if(a==Sopot_n_leds-1){
            led_on_Sopot=0;
          }
          Sopot[led_on_Sopot].dimmingState = 1;
        }
        if(Sopot[a].colorCounter > 1){
          Sopot[a].colorCounter-=1;  
          pixels_1.setPixelColor(Sopot[a].nr_led, pixels_1.Color(Sopot[a].colorCounter,Sopot[a].colorCounter,Sopot[a].colorCounter/2));         
        }
        else if(Sopot[a].colorCounter <= 1){
          Sopot[a].dimmingState = 0;
          pixels_1.setPixelColor(Sopot[a].nr_led, pixels_1.Color(0,0,0));      
        }
      }
    }
    //KRYNICA LIGHTHOUSE
    for(unsigned int a = 0; a < Krynica_n_leds; a = a + 1 ){
      if(Krynica[a].dimmingState == 1){
        if(Krynica[a].colorCounter < Krynica[a].color[0]){
          Krynica[a].colorCounter+=1;
          pixels_1.setPixelColor(Krynica[a].nr_led, pixels_1.Color(Krynica[a].colorCounter,Krynica[a].colorCounter,Krynica[a].colorCounter/2));      
        }
        else if(Krynica[a].colorCounter >= Krynica[a].color[0]){
          Krynica[a].dimmingState = -1;
        }
      }
      if(Krynica[a].dimmingState == -1){
        if(Krynica[a].colorCounter == Krynica[a].color[0]/2){
          if(a<Krynica_n_leds-1){
            led_on_Krynica=a+1;
          }
          else if(a==Krynica_n_leds-1){
            led_on_Krynica=0;
          }
          Krynica[led_on_Krynica].dimmingState = 1;
        }
        if(Krynica[a].colorCounter > 1){
          Krynica[a].colorCounter-=1;  
          pixels_1.setPixelColor(Krynica[a].nr_led, pixels_1.Color(Krynica[a].colorCounter,Krynica[a].colorCounter,Krynica[a].colorCounter/2));         
        }
        else if(Krynica[a].colorCounter <= 1){
          Krynica[a].dimmingState = 0;
          pixels_1.setPixelColor(Krynica[a].nr_led, pixels_1.Color(0,0,0));      
        }
      }
    }
    //SHCHUKINSKIY LIGHTHOUSE
    for(unsigned int a = 0; a < Shchukinskiy_n_leds; a = a + 1 ){
      if(Shchukinskiy[a].dimmingState == 1){
        if(Shchukinskiy[a].colorCounter < Shchukinskiy[a].color[0]){
          Shchukinskiy[a].colorCounter+=1;
          pixels_1.setPixelColor(Shchukinskiy[a].nr_led, pixels_1.Color(Shchukinskiy[a].colorCounter,Shchukinskiy[a].colorCounter,Shchukinskiy[a].colorCounter/2));      
        }
        else if(Shchukinskiy[a].colorCounter >= Shchukinskiy[a].color[0]){
          Shchukinskiy[a].dimmingState = -1;
        }
      }
      if(Shchukinskiy[a].dimmingState == -1){
        if(Shchukinskiy[a].colorCounter == Shchukinskiy[a].color[0]/2){
          if(a<Shchukinskiy_n_leds-1){
            led_on_Shchukinskiy=a+1;
          }
          else if(a==Shchukinskiy_n_leds-1){
            led_on_Shchukinskiy=0;
          }
          Shchukinskiy[led_on_Shchukinskiy].dimmingState = 1;
        }
        if(Shchukinskiy[a].colorCounter > 1){
          Shchukinskiy[a].colorCounter-=1;  
          pixels_1.setPixelColor(Shchukinskiy[a].nr_led, pixels_1.Color(Shchukinskiy[a].colorCounter,Shchukinskiy[a].colorCounter,Shchukinskiy[a].colorCounter/2));         
        }
        else if(Shchukinskiy[a].colorCounter <= 1){
          Shchukinskiy[a].dimmingState = 0;
          pixels_1.setPixelColor(Shchukinskiy[a].nr_led, pixels_1.Color(0,0,0));      
        }
      }
    }
    //OBZORNYY LIGHTHOUSE
    for(unsigned int a = 0; a < Obzornyy_n_leds; a = a + 1 ){
      if(Obzornyy[a].dimmingState == 1){
        if(Obzornyy[a].colorCounter < Obzornyy[a].color[0]){
          Obzornyy[a].colorCounter+=1;
          pixels_1.setPixelColor(Obzornyy[a].nr_led, pixels_1.Color(Obzornyy[a].colorCounter,Obzornyy[a].colorCounter,Obzornyy[a].colorCounter/2));      
        }
        else if(Obzornyy[a].colorCounter >= Obzornyy[a].color[0]){
          Obzornyy[a].dimmingState = -1;
        }
      }
      if(Obzornyy[a].dimmingState == -1){
        if(Obzornyy[a].colorCounter == Obzornyy[a].color[0]/2){
          if(a<Obzornyy_n_leds-1){
            led_on_Obzornyy=a+1;
          }
          else if(a==Obzornyy_n_leds-1){
            led_on_Obzornyy=0;
          }
          Obzornyy[led_on_Obzornyy].dimmingState = 1;
        }
        if(Obzornyy[a].colorCounter > 1){
          Obzornyy[a].colorCounter-=1;  
          pixels_1.setPixelColor(Obzornyy[a].nr_led, pixels_1.Color(Obzornyy[a].colorCounter,Obzornyy[a].colorCounter,Obzornyy[a].colorCounter/2));         
        }
        else if(Obzornyy[a].colorCounter <= 1){
          Obzornyy[a].dimmingState = 0;
          pixels_1.setPixelColor(Obzornyy[a].nr_led, pixels_1.Color(0,0,0));      
        }
      }
    }
    //TARAN LIGHTHOUSE
    for(unsigned int a = 0; a < Taran_n_leds; a = a + 1 ){
      if(Taran[a].dimmingState == 1){
        if(Taran[a].colorCounter < Taran[a].color[0]){
          Taran[a].colorCounter+=1;
          pixels_1.setPixelColor(Taran[a].nr_led, pixels_1.Color(Taran[a].colorCounter,Taran[a].colorCounter,Taran[a].colorCounter/2));      
        }
        else if(Taran[a].colorCounter >= Taran[a].color[0]){
          Taran[a].dimmingState = -1;
        }
      }
      if(Taran[a].dimmingState == -1){
        if(Taran[a].colorCounter == Taran[a].color[0]/2){
          if(a<Taran_n_leds-1){
            led_on_Taran=a+1;
          }
          else if(a==Taran_n_leds-1){
            led_on_Taran=0;
          }
          Taran[led_on_Taran].dimmingState = 1;
        }
        if(Taran[a].colorCounter > 1){
          Taran[a].colorCounter-=1;  
          pixels_1.setPixelColor(Taran[a].nr_led, pixels_1.Color(Taran[a].colorCounter,Taran[a].colorCounter,Taran[a].colorCounter/2));         
        }
        else if(Taran[a].colorCounter <= 1){
          Taran[a].dimmingState = 0;
          pixels_1.setPixelColor(Taran[a].nr_led, pixels_1.Color(0,0,0));      
        }
      }
    }
    //ROZEWIE LIGHTHOUSE
    for(unsigned int a = 0; a < Rozewie_n_leds; a = a + 1 ){
      if(Rozewie[a].dimmingState == 1){
        if(Rozewie[a].colorCounter < Rozewie[a].color[0]){
          Rozewie[a].colorCounter+=1;
          pixels_1.setPixelColor(Rozewie[a].nr_led, pixels_1.Color(Rozewie[a].colorCounter,Rozewie[a].colorCounter,Rozewie[a].colorCounter/2));      
        }
        else if(Rozewie[a].colorCounter >= Rozewie[a].color[0]){
          Rozewie[a].dimmingState = -1;
        }
      }
      if(Rozewie[a].dimmingState == -1){
        if(Rozewie[a].colorCounter == Rozewie[a].color[0]/2){
          if(a<Rozewie_n_leds-1){
            led_on_Rozewie=a+1;
          }
          else if(a==Rozewie_n_leds-1){
            led_on_Rozewie=0;
          }
          Rozewie[led_on_Rozewie].dimmingState = 1;
        }
        if(Rozewie[a].colorCounter > 1){
          Rozewie[a].colorCounter-=1;  
          pixels_1.setPixelColor(Rozewie[a].nr_led, pixels_1.Color(Rozewie[a].colorCounter,Rozewie[a].colorCounter,Rozewie[a].colorCounter/2));         
        }
        else if(Rozewie[a].colorCounter <= 1){
          Rozewie[a].dimmingState = 0;
          pixels_1.setPixelColor(Rozewie[a].nr_led, pixels_1.Color(0,0,0));      
        }
      }
    }
    pixels_1.show();
  }  
}
 