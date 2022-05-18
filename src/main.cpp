#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
  #include <avr/power.h>
#endif


#define PIN_LED1       2 


#define NUMPIXELS      132
#define BUOYS          132


//Types of buoys
uint32_t Q6_LFI_W_15s[14] = {300,700,300,700,300,700,300,700,300,700,300,700,2000,7000};
uint32_t Mo_A_10s[14] = {500,500,1500,7500,0,0,0,0,0,0,0,0,0,0};
uint32_t FI_Y_3s[14] = {1000,2000,0,0,0,0,0,0,0,0,0,0,0,0};
uint32_t LFI_10s[14] = {3000,7000,0,0,0,0,0,0,0,0,0,0,0,0};
uint32_t Iso_10s[14] = {5000,5000,0,0,0,0,0,0,0,0,0,0,0,0};
uint32_t Q[14] = {500,500,0,0,0,0,0,0,0,0,0,0,0,0};
uint32_t Oc_G_10s_9m[14] = {6000,4000,0,0,0,0,0,0,0,0,0,0,0,0};
uint32_t Iso_4s[14] = {2000,2000,0,0,0,0,0,0,0,0,0,0,0,0};
uint32_t DIR[14] = {5000,1,0,0,0,0,0,0,0,0,0,0,0,0};
uint32_t Q_3_10s[14] = {300,700,300,700,300,7700,0,0,0,0,0,0,0,0};
uint32_t Oc_WRG_4s[14] = {2500,1500,0,0,0,0,0,0,0,0,0,0,0,0};

Adafruit_NeoPixel pixels_1 = Adafruit_NeoPixel(NUMPIXELS, PIN_LED1, NEO_GRB + NEO_KHZ800);

 
typedef struct 
{
  int nr;
  int stripe;
  int nr_led;
  int color[3];
  int numOfSeq;
  int counter;
  uint32_t sequence[14];
  uint32_t timer;
} buoy;

typedef struct 
{
  int nr;
  int stripe;
  int nr_led;
  int color;
  int colorCounter;
  int dimmingState; //0 0ff / 1 up / -1 down
} lighthouse;


#include "/home/dymitr/Documents/2022_01-Locja_super_yachts/firmware/esp32-wrover-b/buoy_leds/led_list.txt"

//numer , pasek , numer_led , {kolor} , odstęp między zmianą natężenia
lighthouse lhHel[8] = 
{
{1,1,0,254,0,1},
{2,1,1,254,0,0},
{3,1,2,254,0,0},
{4,1,3,254,0,0},
{5,1,4,254,0,0},
{6,1,5,254,0,0},
{7,1,7,254,0,0},
{8,1,6,254,0,0}
};

lighthouse lhJas[6] = 
{
{1,2,0,254,0,1},
{2,2,1,254,0,0},
{3,2,4,254,0,0},
{4,2,9,254,0,0},
{5,2,8,254,0,0},
{6,2,5,254,0,0}
};

lighthouse lhKas[4] = 
{
{1,2,3,254,0,1},
{2,2,2,254,0,0},
{3,2,6,254,0,0},
{4,2,7,254,0,0}
};

uint32_t lhTimer;
uint32_t now;

int lhdel = 10;
bool helStart = true;
bool jastarniaStart = true;
bool kaszycaStart = true;
int nHel;
int nJastania;
int nKaszyca;

void setup() {

  pixels_1.setBrightness(255);

  pixels_1.begin(); // This initializes the NeoPixel library.


  for(int bid = 0; bid < 132; bid++){
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
}
  //HEL LIGHTHOUSE
  // now = millis();

  // if(now - lhTimer >= lhdel){ 
    
  //   for(unsigned int a = 0; a < 8; a = a + 1 ){
  //     // if(helStart == TRUE){
  //     //   lhHel[a].dimmingState = 1;
  //     //   helStart = FALSE;
  //     // }
  //     if(lhHel[a].dimmingState == 1){
  //       if(lhHel[a].colorCounter < lhHel[a].color){
  //         lhHel[a].colorCounter+=1;
 
      
  //         pixels_1.setPixelColor(lhHel[a].nr_led, pixels_1.Color(lhHel[a].colorCounter,lhHel[a].colorCounter,lhHel[a].colorCounter/2));      
  //         pixels_1.show();    
   
  //         // setLed(lhHel[a].stripe,lhHel[a].nr_led,lhHel[a].colorCounter[0],lhHel[a].colorCounter[1],lhHel[a].colorCounter[2]);
  //       }
  //       else if(lhHel[a].colorCounter >= lhHel[a].color){
  //         lhHel[a].dimmingState = -1;
  //       }
  //     }
  //     if(lhHel[a].dimmingState == -1){
  //       if(lhHel[a].colorCounter == lhHel[a].color/2){
  //         if(a<7){
  //           nHel=a+1;
  //         }
  //         else if(a==7){
  //           nHel=0;
  //         }
  //         lhHel[nHel].dimmingState = 1;
  //       }
  //       if(lhHel[a].colorCounter > 1){
  //         lhHel[a].colorCounter-=1;
          
  //         pixels_1.setPixelColor(lhHel[a].nr_led, pixels_1.Color(lhHel[a].colorCounter,lhHel[a].colorCounter,lhHel[a].colorCounter/2));      
  //         pixels_1.show();    
          
  //         // setLed(lhHel[a].stripe,lhHel[a].nr_led,lhHel[a].colorCounter,lhHel[a].colorCounter,lhHel[a].colorCounter);
  //       }
  //       else if(lhHel[a].colorCounter <= 1){
  //         lhHel[a].dimmingState = 0;
  //       }
  //     }
  //   }
  //   // lhTimer = millis();
  // }  
  // //Jastarnia LIGHTHOUSE
  // // now = millis();

  // if(now - lhTimer >= lhdel){ 
    
  //   for(unsigned int a = 0; a < 6; a = a + 1 ){
  //     if(lhJas[a].dimmingState == 1){
  //       if(lhJas[a].colorCounter < lhJas[a].color){
  //         lhJas[a].colorCounter+=1;

  //         pixels_2.setPixelColor(lhJas[a].nr_led, pixels_2.Color(lhJas[a].colorCounter,lhJas[a].colorCounter,lhJas[a].colorCounter/2));      
  //         pixels_2.show();  
  //         // setLed(lhJas[a].stripe,lhJas[a].nr_led,lhJas[a].colorCounter,lhJas[a].colorCounter,lhJas[a].colorCounter);
  //       }
  //       else if(lhJas[a].colorCounter >= lhJas[a].color){
  //         lhJas[a].dimmingState = -1;
  //         if(a<5){
  //           nHel=a+1;
  //         }
  //         else if(a==5){
  //           nHel=0;
  //         }
  //         lhJas[nHel].dimmingState = 1;
  //       }
  //     }
  //     if(lhJas[a].dimmingState == -1){
  //       if(lhJas[a].colorCounter > 1){
  //         lhJas[a].colorCounter-=2;
  //         pixels_2.setPixelColor(lhJas[a].nr_led, pixels_2.Color(lhJas[a].colorCounter,lhJas[a].colorCounter,lhJas[a].colorCounter/2));      
  //         pixels_2.show();  
  //         // setLed(lhJas[a].stripe,lhJas[a].nr_led,lhJas[a].colorCounter,lhJas[a].colorCounter,lhJas[a].colorCounter);
  //       }
  //       else if(lhJas[a].colorCounter <= 1){
  //         lhJas[a].dimmingState = 0;
  //       }
  //     }
  //   }
  //   // lhTimer = millis();
  // }  
  // // Kaszyca LIGHTHOUSE
  // // now = millis();

  // if(now - lhTimer >= lhdel){ 
    
  //   for(unsigned int a = 0; a < 4; a = a + 1 ){
  //     if(lhKas[a].dimmingState == 1){
  //       if(lhKas[a].colorCounter < lhKas[a].color){
  //         lhKas[a].colorCounter+=1;
  //         pixels_2.setPixelColor(lhKas[a].nr_led, pixels_2.Color(lhKas[a].colorCounter,lhKas[a].colorCounter,lhKas[a].colorCounter/2));      
  //         pixels_2.show();  
  //         // setLed(lhKas[a].stripe,lhKas[a].nr_led,lhKas[a].colorCounter,lhKas[a].colorCounter,lhKas[a].colorCounter);
  //       }
  //       else if(lhKas[a].colorCounter >= lhKas[a].color){
  //         lhKas[a].dimmingState = -1;
  //         if(a<3){
  //           nHel=a+1;
  //         }
  //         else if(a==3){
  //           nHel=0;
  //         }
  //         lhKas[nHel].dimmingState = 1;
  //       }
  //     }
  //     if(lhKas[a].dimmingState == -1){
  //       if(lhKas[a].colorCounter > 1){
  //         lhKas[a].colorCounter-=2;
  //         pixels_2.setPixelColor(lhKas[a].nr_led, pixels_2.Color(lhKas[a].colorCounter,lhKas[a].colorCounter,lhKas[a].colorCounter/2));      
  //         pixels_2.show(); 
  //         // setLed(lhKas[a].stripe,lhKas[a].nr_led,lhKas[a].colorCounter,lhKas[a].colorCounter,lhKas[a].colorCounter);
  //       }
  //       else if(lhKas[a].colorCounter <= 1){
  //         lhKas[a].dimmingState = 0;
  //       }
  //     }
  //   }
  //   lhTimer = millis();
  // }  

