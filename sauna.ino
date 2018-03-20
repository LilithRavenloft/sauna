#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"
#include <Ticker.h>
 
#define WIFI_SSID ""
#define WIFI_PASS ""
#define SERIAL_BAUDRATE                 115200
 
Ticker ticker;
fauxmoESP fauxmo;
bool status = false; // Code muss sich merken, ob das System gerade an oder aus ist. Initial auf false = aus
bool procedureReady = true;
 
// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------
 
void wifiSetup() {
   
    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);
 
    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
 
    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();
 
    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
 
}
 
void timerCallbackWakeUp(){
  Serial.println("AUS");
  digitalWrite(D7, LOW);
  ticker.once_ms(3000, timerCallbackAN);
}
 
void timerCallbackAN(){
    Serial.println("AN");
    digitalWrite(D7, HIGH);
    ticker.once_ms(5000, timerCallbackAUS);
}
 
void timerCallbackAUS(){
    Serial.println("AUS");
    digitalWrite(D7, LOW);
    procedureReady = true;
}
 
void tasterCallback(){
  if(procedureReady){
     procedureReady = false;
     if(!status){
        Serial.println("AN");
        digitalWrite(D7, HIGH);
        ticker.once_ms(1500, timerCallbackWakeUp);
        status = true;
     }
     else{
        Serial.println("AN");
        digitalWrite(D7, HIGH);
        ticker.once_ms(1000, timerCallbackAUS);
        status = false;
     }
  }
}
 
void callback(uint8_t device_id, const char * device_name, bool state) {
  Serial.print("Gerät "); Serial.print(device_name);
  Serial.print(" Status: ");
  if (state) {
  status = true;
    Serial.println("AN");
    digitalWrite(D7, HIGH);
    ticker.once_ms(1500, timerCallbackWakeUp);
  } else {
    status = false;
    Serial.println("AN");
    digitalWrite(D7, HIGH);
    ticker.once_ms(1000, timerCallbackAUS);
  }
}
 
 
void setup() {
    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
   
    // PIN D7 Initialisieren    
    pinMode(D7, OUTPUT);
    //Pin D7 auf LOW setzten um bei Stromausfall (bzw. wiederkehr des Stroms) den Ausgang auf "AUS" zu halten
    digitalWrite(D7, LOW);
   
    // Wifi
    wifiSetup();
 
    // PIN D8 Initialisieren
    pinMode(D8, INPUT_PULLUP);
    attachInterrupt(D8, tasterCallback, CHANGE);
   
    // Fauxmo
    fauxmo.addDevice("Testgerät");
    fauxmo.onMessage(callback);
}
 
void loop() {
  fauxmo.handle();
  }