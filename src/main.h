#pragma once

#include <EEPROM.h>

#define CONFIG_HEADER 0xDEAD
#define CONFIG_VERSION 0

struct config_t{
  uint16_t header ;
  uint8_t conf_id ;

  char  hostname  [32];
  char  ssid  [32];
  char  ssid_password  [32];
  //char  ap_ssid  [32];
  char  ap_ssid_password  [32];
  uint8_t brightness[3];
};


extern bool reboot;
extern bool client_has_connected;
extern config_t config;

void config_set_defaults();
void config_read();
void config_write();
void config_print();
void wifi_connect();

void httpd_setup();

String wifiGetMac();