#include "Arduino.h"

#include "Adafruit_NeoPixel.h"

#ifdef __AVR__
  #include <avr/power.h>
#endif

// Import required libraries
#ifdef ESP32
  #include <WiFi.h>
  #include <ESPmDNS.h>
  #include <AsyncTCP.h>
  #include <esp_wifi.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include "main.h"
#include "ota.h"
#include "map_leds.h"

#define PIN_LED1       2 

Adafruit_NeoPixel pixels_1 = Adafruit_NeoPixel(NUMPIXELS, PIN_LED1, NEO_GRB + NEO_KHZ800);

// default config 
config_t config = {}; 

bool reboot = false;
bool client_has_connected = false;

AsyncWebServer httpd(80);

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  EEPROM.begin(1024);


  config_set_defaults();
  config_read();
  config_print();

  wifi_connect();

  handle_leds_setup();


  httpd_setup();

  AsyncElegantOTA.begin(&httpd, "admin", config.ap_ssid_password);
  setup_ota();
}


void loop() {

  static unsigned long last_connected = 0;
  if (last_connected == 0){ last_connected = millis(); };

  static unsigned long last_status_display = 0;
  if (last_status_display == 0){ last_status_display = millis(); };

  static bool any_connected_to_ap = false;

  loop_ota();

  handle_leds();  

  if ((WiFi.getMode() & WIFI_AP )== WIFI_AP){

    if (WiFi.softAPgetStationNum() > 0){
      last_connected = millis();
      any_connected_to_ap = true;
    }

    if (millis() - last_connected > (any_connected_to_ap ? 5000 : 60000)){
      reboot  = true;
    }

  } else 
  if ((WiFi.getMode() & WIFI_STA )== WIFI_STA){
    if (WiFi.status() == WL_CONNECTED) {
      last_connected = millis();
    }else{
      if (millis() - last_connected > 10000){
        reboot  = true;
      }
    }
  } else {

    Serial.println("WTF? not STA and not AP? rebooting");
    reboot = true;
  }

  if (reboot){
    delay(500);
    ESP.restart();
  }

  if (millis() - last_status_display > 1000){
    Serial.print("Status:");
    Serial.print("wifi_mode=");
    if((WiFi.getMode() & WIFI_STA )== WIFI_STA) {
      Serial.print("STA");
      Serial.print(",wifi_sta_conected=");
      if(WiFi.status() == WL_CONNECTED){
        Serial.print("yes");
      } else {
        Serial.print("no");
      }

    }
    if((WiFi.getMode() & WIFI_AP) == WIFI_AP) {
      Serial.print("AP");
      Serial.print(",clients=");
      Serial.print(WiFi.softAPgetStationNum());
    }
    Serial.print(",last_connected=");
    Serial.print((millis() - last_connected)/1000 );
    Serial.println();
    last_status_display = millis();
  }

}

void wifi_connect(){
  WiFi.hostname(config.hostname);
  WiFi.mode(WIFI_STA);
  WiFi.begin(config.ssid, config.ssid_password);

  Serial.print("Trying connect as STA to:");
  Serial.println(config.ssid);
  Serial.print("With pass: ");
  Serial.println(config.ssid_password);

  for (int i =0 ; i< 20; i++){
    if (WiFi.status() == WL_CONNECTED) break;
    delay(100);
    Serial.print(".");
  } 

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected");

    if (MDNS.begin(config.hostname)) Serial.printf("mDNS responder started\r\n\tName: %s.local\r\n", config.hostname);
    MDNS.addService("http", "tcp", 80);

  } else {
    WiFi.disconnect (true);

    Serial.println("Not connected");

    Serial.print("Unable to connect as STA to:");
    Serial.println(config.ssid);
    Serial.print("with pass: ");
    Serial.println(config.ssid_password);

    IPAddress ip(192, 168, 7, 1);
    IPAddress gateway(192, 168, 7, 254);
    IPAddress subnet(255, 255, 255, 0);

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(ip, gateway, subnet);
    WiFi.softAP((String("OBMapa-") + wifiGetMac()).c_str(), (const char *) config.ap_ssid_password);

    //dnsServer.setTTL(300);
    // set which return code will be used for all other domains (e.g. sending
    // ServerFailure instead of NonExistentDomain will reduce number of queries
    // sent by clients)
    // default is DNSReplyCode::NonExistentDomain
    //dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
    // start DNS server for a specific domain name
    //dnsServer.start(53, "led.ob", ip);

    Serial.println("Setup AP");
    // Wait for connection
  }

  httpd.begin();

  Serial.println("");
}

static config_t config_def = { 
    //.header = 
    CONFIG_HEADER, 
    //.conf_id = 
    CONFIG_VERSION,
    //.hostname = 
    "OBMapa", 
    //.ssid = 
    DEF_SSID, 
    //.ssid_password = 
    DEF_SSID_PASSWORD,
    //.ap_ssid_password = 
    DEF_AP_SSID_PASSWORD,
    //.brightness = 
    {10,127,254}
  };

void config_set_defaults(){
  config = config_def;
}

void config_read(){

  config_t new_config;

  EEPROM.get(0,new_config);

  if (new_config.header != 0xDEAD || new_config.conf_id != CONFIG_VERSION){

    Serial.println("Saved config is broken, not setting");

  } else {
    Serial.println("Config read");
    config = new_config;
  }

}

void config_write(){
  Serial.println("Writting config");
  EEPROM.put(0,config);
  EEPROM.commit();
}

void config_print(){
  Serial.print("config.hostname=");Serial.println(config.hostname);
  Serial.print("config.ssid=");Serial.println(config.ssid);
  Serial.print("config.ssid_password=");Serial.println(config.ssid_password);
  Serial.print("config.ap_ssid_password=");Serial.println(config.ap_ssid_password);
}

String wifiGetMac(){
    uint8_t mac[6];
    char macStr[18] = { 0 };

#ifdef ESP32
    esp_wifi_get_mac(WIFI_IF_STA, mac);
#else    
    // esp8266
    wifi_get_macaddr(STATION_IF, mac);
#endif    

    sprintf(macStr, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(macStr);
}


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Mapa</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.6rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 10px;}

    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>Mapa</h2>
  <button onclick="ledsOff()">ledsOff</button> <br/>
  <button onclick="ledsB0()">led Brightness 0</button> <br/>
  <button onclick="ledsB1()">led Brightness 1</button> <br/>
  <button onclick="ledsB2()">led Brightness 2</button> <br/>
  <br/>
  <br/>
  <button onclick="reboot()">reboot</button>
  <!--<button onclick="logoutButton()">Logout</button>-->
<script>
function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ 
    xhr.open("GET", "/update?state=1", true); 
    document.getElementById("state").innerHTML = "ON";  
  }
  else { 
    xhr.open("GET", "/update?state=0", true); 
    document.getElementById("state").innerHTML = "OFF";      
  }
  xhr.send();
}


function ledsOff(element) {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/led_off", true);
  xhr.send();
}

function ledsB0(element) {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/led_b0", true);
  xhr.send();
}

function ledsB1(element) {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/led_b1", true);
  xhr.send();
}

function ledsB2(element) {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/led_b2", true);
  xhr.send();
}

function reboot(element) {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/reboot", true);
  xhr.send();
}

function logoutButton() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/logout", true);
  xhr.send();
  setTimeout(function(){ window.open("/logged-out","_self"); }, 1000);
}
</script>
</body>
</html>
)rawliteral";


const char config_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Mapa Config</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.6rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 10px;}

    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>

<body>
  <h2>Mapa config</h2>

  <button onclick="homeBtn()">home</button>

 <form action="/config" method="post">
  <label for="hostname">Hostname:</label><br>
  <input type="text" id="hostname" name="hostname" value="%CONFIG_HOSTNAME%"><br>
  
  <label for="ssid">STA SSID:</label><br>
  <input type="text" id="ssid" name="ssid" value="%CONFIG_SSID%"><br>

  <label for="ssidpassword">STA password:</label><br>
  <input type="text" id="ssidpassword" name="ssidpassword" value="%CONFIG_SSID_PASSWORD%"><br>

  <label for="apssidpassword">AP password:</label><br>
  <input type="text" id="apssidpassword" name="apssidpassword" value="%CONFIG_AP_SSID_PASSWORD%"><br>

  <label for="brightness0">Brightness 0 (0-255):</label><br>
  <input type="text" id="brightness0" name="brightness0" value="%CONFIG_BRIGHTNESS_0%"><br>

  <label for="brightness1">Brightness 1 (0-255):</label><br>
  <input type="text" id="brightness1" name="brightness1" value="%CONFIG_BRIGHTNESS_1%"><br>

  <label for="brightness2">Brightness 2 (0-255):</label><br>
  <input type="text" id="brightness2" name="brightness2" value="%CONFIG_BRIGHTNESS_2%"><br>
  
  <input type="submit" value="Sumbit">
 </form>

 <button onclick="saveAndReboot()">SaveAndReboot</button>

<script>


function saveAndReboot(element) {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/save_and_reboot", true);
  xhr.send();
  setTimeout(function(){ window.open("/","_self"); }, 3000);
}

function homeBtn() {
  setTimeout(function(){ window.open("/","_self"); }, 100);
}

</script>
</body>
</html>
)rawliteral";


const char logout_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
  <p>Logged out or <a href="/">return to homepage</a>.</p>
  <p><strong>Note:</strong> close all web browser tabs to complete the logout process.</p>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
/*
  if(var == "BUTTONPLACEHOLDER"){
    String buttons ="";
    String outputStateValue = outputState();
    buttons+= "<p><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"output\" " + outputStateValue + "><span class=\"slider\"></span></label></p>";
    return buttons;
  }
  if (var == "STATE"){
    if(digitalRead(output)){
      return "ON";
    }
    else {
      return "OFF";
    }
  }
*/
  return String();
}

String config_processor(const String& var){

  if (var == "CONFIG_HOSTNAME") return config.hostname;
  if (var == "CONFIG_SSID") return config.ssid;
  if (var == "CONFIG_SSID_PASSWORD") return config.ssid_password;
  if (var == "CONFIG_AP_SSID_PASSWORD") return config.ap_ssid_password;
  if (var == "CONFIG_BRIGHTNESS_0") return String(config.brightness[0]);
  if (var == "CONFIG_BRIGHTNESS_1") return String(config.brightness[1]);
  if (var == "CONFIG_BRIGHTNESS_2") return String(config.brightness[2]);

  return String(var);
}

void httpd_setup(){
  // Route for root / web page
  httpd.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate("admin", config.ap_ssid_password))
      return request->requestAuthentication();
    request->send(200, "text/html", index_html);
  });

  httpd.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(401);
  });

  httpd.on("/logged-out", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", logout_html);
  });

  httpd.on("/led_off", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "OK\r\n");
    pixels_1.setBrightness(0);
  });

  httpd.on("/led_b0", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "OK\r\n");
    pixels_1.setBrightness(config.brightness[0]);
  });

  httpd.on("/led_b1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "OK\r\n");
    pixels_1.setBrightness(config.brightness[1]);
  });

  httpd.on("/led_b2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "OK\r\n");
    pixels_1.setBrightness(config.brightness[2]);
  });


  httpd.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "OK\r\n");
    reboot = true;
  });

  httpd.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", config_html,config_processor);
  });


  httpd.on("/save_and_reboot", HTTP_GET, [](AsyncWebServerRequest *request){
    config_write();
    reboot = true;
    request->send(200, "text/html", "OK\r\n");
  });



  httpd.on("/config", HTTP_POST, [](AsyncWebServerRequest *request){

    if(request->hasArg("hostname")){
      String hostname = request->arg("hostname");
      Serial.print("hostname:");
      Serial.println(hostname);
      if(hostname.length() >0 && hostname.length() < 32){
        strncpy(config.hostname, hostname.c_str(), 31);
      }
    }

    if(request->hasArg("ssid")){
      String ssid = request->arg("ssid");
      Serial.print("ssid:");
      Serial.println(ssid);

      if(ssid.length() >0 && ssid.length() < 32){
        strncpy(config.ssid, ssid.c_str(), 31);
      }

    }

    if(request->hasArg("ssidpassword")){
      String ssid_password = request->arg("ssidpassword");
      Serial.print("ssid_password:");
      Serial.println(ssid_password);

      if(ssid_password.length()> 7 && ssid_password.length() < 32){
        strncpy(config.ssid_password, ssid_password.c_str(), 31);
      }
    }

    if(request->hasArg("apssidpassword")){
      String ap_ssid_password = request->arg("apssidpassword");
      Serial.print("ap_ssid_password:");
      Serial.println(ap_ssid_password);

      if(ap_ssid_password.length() > 0 && ap_ssid_password.length() < 32){
        strncpy(config.ap_ssid_password, ap_ssid_password.c_str(), 31);
      }

    }

    if(request->hasArg("brightness0")){
      String brightness0 = request->arg("brightness0");
      Serial.print("brightness0:");
      Serial.println(brightness0);
      config.brightness[0] = brightness0.toInt();
    }

    if(request->hasArg("brightness1")){
      String brightness1 = request->arg("brightness1");
      Serial.print("brightness1:");
      Serial.println(brightness1);
      config.brightness[1] = brightness1.toInt();
    }

    if(request->hasArg("brightness2")){
      String brightness2 = request->arg("brightness2");
      Serial.print("brightness2:");
      Serial.println(brightness2);
      config.brightness[2] = brightness2.toInt();
    }

    request->send_P(200, "text/html", config_html,config_processor);
  });

}
