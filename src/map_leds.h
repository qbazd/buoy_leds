#pragma once

#include "Arduino.h"
#include "Adafruit_NeoPixel.h"

#define NUMPIXELS      132
#define BUOYS          132

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


extern uint32_t lhTimer;
extern uint32_t now;
extern int lhdel; //delay for all lighthouse
extern int led_on_Jastarnia;
extern int led_on_Kaszyca;
extern int led_on_Hel;
extern int led_on_Gdansk;
extern int led_on_Sopot;
extern int led_on_Krynica;
extern int led_on_Shchukinskiy;
extern int led_on_Obzornyy;
extern int led_on_Taran;
extern int led_on_Rozewie;
extern Adafruit_NeoPixel pixels_1 ;

void setLed(int stripeNr, int ledNr, int R, int G, int B);
void handle_leds_setup();
void handle_leds();
