#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
  #include <avr/power.h>
#endif


#define PIN_LED1            4 //latarnia morska HEL
#define PIN_LED2            5 //latarnie morskie JASTARNIA  
#define PIN_LED3            6
#define PIN_LED4            7 

#define NUMPIXELS      10

// int pixels_1_green[10] = {0,1,2,3,4,5,6,7,8,9};
// int pixels_2_green[10] = {0,1,2,3,4,5,6,7,8,9};
// pixels_3_green = []
// pixels_4_green = []

// pixels_1_red = []
// pixels_2_red = []
// pixels_3_red = []
// pixels_4_red = []

// int pixels_1_white[8] = {0,1,2,3,4,5,6,7};
// int pixels_2_white[10] = {0,1,2,3,4,5,6,7,8,9};
// pixels_3_white = []
// pixels_4_white = []
 
// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels_1 = Adafruit_NeoPixel(8, PIN_LED1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_2 = Adafruit_NeoPixel(NUMPIXELS, PIN_LED2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_3 = Adafruit_NeoPixel(NUMPIXELS, PIN_LED3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_4 = Adafruit_NeoPixel(NUMPIXELS, PIN_LED4, NEO_GRB + NEO_KHZ800);
 
int delayval = 4000; // delay for half a second
 
typedef struct 
{
  int nr;
  int stripe;
  int nr_led;
  int color[3];
  int numOfSeq;
  int counter;
  uint32_t sequence[14];
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

buoy buoyList[18] = 
{
{1,3,5,{255,150,0},13,0,{300,700,300,700,300,700,300,700,300,700,300,700,2000,7000}},
{2,3,7,{255,150,0},3,0,{500,500,1500,7500,0,0,0,0,0,0,0,0,0,0}},
{3,3,9,{255,150,0},1,0,{1000,2000,0,0,0,0,0,0,0,0,0,0,0,0}},
{4,3,8,{255,150,0},1,0,{3000,7000,0,0,0,0,0,0,0,0,0,0,0,0}},
{5,3,0,{255,150,0},1,0,{1000,2000,0,0,0,0,0,0,0,0,0,0,0,0}},
{6,3,4,{255,150,0},1,0,{5000,5000,0,0,0,0,0,0,0,0,0,0,0,0}},
{7,3,3,{255,150,0},1,0,{500,500,0,0,0,0,0,0,0,0,0,0,0,0}},
{8,3,6,{0,100,0},1,0,{6000,4000,0,0,0,0,0,0,0,0,0,0,0,0}},
{9,3,2,{255,0,0},1,0,{2000,2000,0,0,0,0,0,0,0,0,0,0,0,0}},
{10,3,1,{0,100,0},1,0,{2000,2000,0,0,0,0,0,0,0,0,0,0,0,0}},
{11,4,2,{255,0,0},1,0,{5000,1,0,0,0,0,0,0,0,0,0,0,0,0}}, //WRG
{12,4,4,{255,150,0},5,0,{300,700,300,700,300,7700,0,0,0,0,0,0,0,0}},
{13,4,5,{255,150,0},5,0,{300,700,300,700,300,7700,0,0,0,0,0,0,0,0}},
{14,4,6,{255,0,0},1,0,{2500,1500,0,0,0,0,0,0,0,0,0,0,0,0}}, //WRG
{15,4,7,{255,150,0},1,0,{3000,7000,0,0,0,0,0,0,0,0,0,0,0,0}},
{16,4,8,{255,150,0},1,0,{3000,7000,0,0,0,0,0,0,0,0,0,0,0,0}},
{17,4,9,{255,150,0},1,0,{3000,7000,0,0,0,0,0,0,0,0,0,0,0,0}},
{18,4,3,{255,150,0},1,0,{3000,7000,0,0,0,0,0,0,0,0,0,0,0,0}}
};

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

uint32_t buoyTimer[18];
// uint32_t lhHelTimer;
// uint32_t lhJasTimer;
// uint32_t lhKasTimer;
uint32_t lhTimer;
uint32_t now;
// int lhHeldel = 10;
// int lhJasdel = 10;
// int lhKasdel = 10;
int lhdel = 10;
bool helStart = true;
bool jastarniaStart = true;
bool kaszycaStart = true;
int nHel;
int nJastania;
int nKaszyca;

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
// #if defined (__AVR_ATtiny85__)
//   if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
// #endif
  // End of trinket special code
  pixels_1.setBrightness(255);
  pixels_2.setBrightness(255);
  pixels_3.setBrightness(255);
  pixels_4.setBrightness(255);
  pixels_1.begin(); // This initializes the NeoPixel library.
  pixels_2.begin(); // This initializes the NeoPixel library.
  pixels_3.begin(); // This initializes the NeoPixel library.
  pixels_4.begin(); // This initializes the NeoPixel library.

  buoyTimer[0],buoyTimer[1],buoyTimer[2],buoyTimer[3],buoyTimer[4],buoyTimer[5],buoyTimer[6],buoyTimer[7],buoyTimer[8],buoyTimer[9],buoyTimer[10],buoyTimer[11],
  buoyTimer[12],buoyTimer[13],buoyTimer[14],buoyTimer[15],buoyTimer[16],buoyTimer[17] = millis();

  // lhHelTimer = millis();
  // lhJasTimer = millis();
  // lhKasTimer = millis();
  lhTimer = millis();
  // uint32_t now = millis();
}
 
// void setLed(int stripeNr, int ledNr, int R, int G, int B){
//   if(stripeNr == 3){
//     pixels_3.setPixelColor(ledNr, pixels_3.Color(R,G,B)); 
//     pixels_3.show();     
//   }
//   else if(stripeNr == 4){
//     pixels_4.setPixelColor(ledNr, pixels_4.Color(R,G,B));      
//     pixels_4.show();    
//   }
//   else if(stripeNr == 1){
//     pixels_1.setPixelColor(ledNr, pixels_1.Color(R,G,B));      
//     pixels_1.show();    
//   }
//   else if(stripeNr == 2){
//     pixels_2.setPixelColor(ledNr, pixels_2.Color(R,G,B));      
//     pixels_2.show();    
//   }
// }
void loop() {
  
  //BUOYS
  now = millis();
  for(unsigned int a = 0; a < 18; a = a + 1 ){
  
    if(buoyList[a].counter == 0){
      if(buoyList[a].stripe == 3){
        pixels_3.setPixelColor(buoyList[a].nr_led, pixels_3.Color(buoyList[a].color[0],buoyList[a].color[1],buoyList[a].color[2])); 
        pixels_3.show();     
      }
      else if(buoyList[a].stripe == 4){
        pixels_4.setPixelColor(buoyList[a].nr_led, pixels_4.Color(buoyList[a].color[0],buoyList[a].color[1],buoyList[a].color[2]));      
        pixels_4.show();    
      }
      else if(buoyList[a].stripe == 1){
        pixels_1.setPixelColor(buoyList[a].nr_led, pixels_1.Color(buoyList[a].color[0],buoyList[a].color[1],buoyList[a].color[2]));      
        pixels_1.show();    
      }
      else if(buoyList[a].stripe == 2){
        pixels_2.setPixelColor(buoyList[a].nr_led, pixels_2.Color(buoyList[a].color[0],buoyList[a].color[1],buoyList[a].color[2]));      
        pixels_2.show();    
      }
      // setLed(buoyList[a].stripe,buoyList[a].nr_led,buoyList[a].color[0],buoyList[a].color[1],buoyList[a].color[2]);
    }
    if(now - buoyTimer[a] >= buoyList[a].sequence[buoyList[a].counter]){
      if(buoyList[a].counter == buoyList[a].numOfSeq){
        buoyList[a].counter = 0;
      }
      else buoyList[a].counter += 1;
  
      if(buoyList[a].counter % 2 == 0){
        if(buoyList[a].stripe == 3){
          pixels_3.setPixelColor(buoyList[a].nr_led, pixels_3.Color(buoyList[a].color[0],buoyList[a].color[1],buoyList[a].color[2])); 
          pixels_3.show();     
        }
        else if(buoyList[a].stripe == 4){
          pixels_4.setPixelColor(buoyList[a].nr_led, pixels_4.Color(buoyList[a].color[0],buoyList[a].color[1],buoyList[a].color[2]));      
          pixels_4.show();    
        }
        else if(buoyList[a].stripe == 1){
          pixels_1.setPixelColor(buoyList[a].nr_led, pixels_1.Color(buoyList[a].color[0],buoyList[a].color[1],buoyList[a].color[2]));      
          pixels_1.show();    
        }
        else if(buoyList[a].stripe == 2){
          pixels_2.setPixelColor(buoyList[a].nr_led, pixels_2.Color(buoyList[a].color[0],buoyList[a].color[1],buoyList[a].color[2]));      
          pixels_2.show();    
        }
        // setLed(buoyList[a].stripe,buoyList[a].nr_led,buoyList[a].color[0],buoyList[a].color[1],buoyList[a].color[2]);
      }
      else{
        if(buoyList[a].stripe == 3){
          pixels_3.setPixelColor(buoyList[a].nr_led, pixels_3.Color(1,1,1)); 
          pixels_3.show();     
        }
        else if(buoyList[a].stripe == 4){
          pixels_4.setPixelColor(buoyList[a].nr_led, pixels_4.Color(1,1,1));      
          pixels_4.show();    
        }
        else if(buoyList[a].stripe == 1){
          pixels_1.setPixelColor(buoyList[a].nr_led, pixels_1.Color(1,1,1));      
          pixels_1.show();    
        }
        else if(buoyList[a].stripe == 2){
          pixels_2.setPixelColor(buoyList[a].nr_led, pixels_2.Color(1,1,1));      
          pixels_2.show();    
        }
        // setLed(buoyList[a].stripe,buoyList[a].nr_led,1,1,1);
      }
      buoyTimer[a] = millis();
    }
  }
  //HEL LIGHTHOUSE
  // now = millis();

  if(now - lhTimer >= lhdel){ 
    
    for(unsigned int a = 0; a < 8; a = a + 1 ){
      // if(helStart == TRUE){
      //   lhHel[a].dimmingState = 1;
      //   helStart = FALSE;
      // }
      if(lhHel[a].dimmingState == 1){
        if(lhHel[a].colorCounter < lhHel[a].color){
          lhHel[a].colorCounter+=1;
 
      
          pixels_1.setPixelColor(lhHel[a].nr_led, pixels_1.Color(lhHel[a].colorCounter,lhHel[a].colorCounter,lhHel[a].colorCounter/2));      
          pixels_1.show();    
   
          // setLed(lhHel[a].stripe,lhHel[a].nr_led,lhHel[a].colorCounter[0],lhHel[a].colorCounter[1],lhHel[a].colorCounter[2]);
        }
        else if(lhHel[a].colorCounter >= lhHel[a].color){
          lhHel[a].dimmingState = -1;
        }
      }
      if(lhHel[a].dimmingState == -1){
        if(lhHel[a].colorCounter == lhHel[a].color/2){
          if(a<7){
            nHel=a+1;
          }
          else if(a==7){
            nHel=0;
          }
          lhHel[nHel].dimmingState = 1;
        }
        if(lhHel[a].colorCounter > 1){
          lhHel[a].colorCounter-=1;
          
          pixels_1.setPixelColor(lhHel[a].nr_led, pixels_1.Color(lhHel[a].colorCounter,lhHel[a].colorCounter,lhHel[a].colorCounter/2));      
          pixels_1.show();    
          
          // setLed(lhHel[a].stripe,lhHel[a].nr_led,lhHel[a].colorCounter,lhHel[a].colorCounter,lhHel[a].colorCounter);
        }
        else if(lhHel[a].colorCounter <= 1){
          lhHel[a].dimmingState = 0;
        }
      }
    }
    // lhTimer = millis();
  }  
  //Jastarnia LIGHTHOUSE
  // now = millis();

  if(now - lhTimer >= lhdel){ 
    
    for(unsigned int a = 0; a < 6; a = a + 1 ){
      if(lhJas[a].dimmingState == 1){
        if(lhJas[a].colorCounter < lhJas[a].color){
          lhJas[a].colorCounter+=1;

          pixels_2.setPixelColor(lhJas[a].nr_led, pixels_2.Color(lhJas[a].colorCounter,lhJas[a].colorCounter,lhJas[a].colorCounter/2));      
          pixels_2.show();  
          // setLed(lhJas[a].stripe,lhJas[a].nr_led,lhJas[a].colorCounter,lhJas[a].colorCounter,lhJas[a].colorCounter);
        }
        else if(lhJas[a].colorCounter >= lhJas[a].color){
          lhJas[a].dimmingState = -1;
          if(a<5){
            nHel=a+1;
          }
          else if(a==5){
            nHel=0;
          }
          lhJas[nHel].dimmingState = 1;
        }
      }
      if(lhJas[a].dimmingState == -1){
        if(lhJas[a].colorCounter > 1){
          lhJas[a].colorCounter-=2;
          pixels_2.setPixelColor(lhJas[a].nr_led, pixels_2.Color(lhJas[a].colorCounter,lhJas[a].colorCounter,lhJas[a].colorCounter/2));      
          pixels_2.show();  
          // setLed(lhJas[a].stripe,lhJas[a].nr_led,lhJas[a].colorCounter,lhJas[a].colorCounter,lhJas[a].colorCounter);
        }
        else if(lhJas[a].colorCounter <= 1){
          lhJas[a].dimmingState = 0;
        }
      }
    }
    // lhTimer = millis();
  }  
  // Kaszyca LIGHTHOUSE
  // now = millis();

  if(now - lhTimer >= lhdel){ 
    
    for(unsigned int a = 0; a < 4; a = a + 1 ){
      if(lhKas[a].dimmingState == 1){
        if(lhKas[a].colorCounter < lhKas[a].color){
          lhKas[a].colorCounter+=1;
          pixels_2.setPixelColor(lhKas[a].nr_led, pixels_2.Color(lhKas[a].colorCounter,lhKas[a].colorCounter,lhKas[a].colorCounter/2));      
          pixels_2.show();  
          // setLed(lhKas[a].stripe,lhKas[a].nr_led,lhKas[a].colorCounter,lhKas[a].colorCounter,lhKas[a].colorCounter);
        }
        else if(lhKas[a].colorCounter >= lhKas[a].color){
          lhKas[a].dimmingState = -1;
          if(a<3){
            nHel=a+1;
          }
          else if(a==3){
            nHel=0;
          }
          lhKas[nHel].dimmingState = 1;
        }
      }
      if(lhKas[a].dimmingState == -1){
        if(lhKas[a].colorCounter > 1){
          lhKas[a].colorCounter-=2;
          pixels_2.setPixelColor(lhKas[a].nr_led, pixels_2.Color(lhKas[a].colorCounter,lhKas[a].colorCounter,lhKas[a].colorCounter/2));      
          pixels_2.show(); 
          // setLed(lhKas[a].stripe,lhKas[a].nr_led,lhKas[a].colorCounter,lhKas[a].colorCounter,lhKas[a].colorCounter);
        }
        else if(lhKas[a].colorCounter <= 1){
          lhKas[a].dimmingState = 0;
        }
      }
    }
    lhTimer = millis();
  }  
}
