#include <Arduino.h>

#include "ota.h"


#ifndef OTA_PASS 
#error "OTA_PASS not defined"
#endif
void setup_ota(){

}

void loop_ota(){
  //ArduinoOTA.handle();
}


#if 0
#include <ArduinoOTA.h>
void setup_ota(){
  // Setup OTA   

  // Port defaults to 8266
  ArduinoOTA.setPort(3232);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname(config.hostname);

  // No authentication by default
  ArduinoOTA.setPassword(OTA_PASS);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");

    // stop leds

  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
}

void loop_ota(){
  ArduinoOTA.handle();
}
#endif
