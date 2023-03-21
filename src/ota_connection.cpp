#include "ota_connection.hpp"
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <TelnetStream.h>


inline void beginWifiAP() {
    WiFi.mode(WIFI_STA);
    WiFi.begin("Feldakku", ""); 

    Serial.print("Creating WiFi Access Point ");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Created WiFi Access Point ");
    Serial.println(WiFi.localIP());
}

inline void beginArduinoOTA() {
    ArduinoOTA.setHostname("Feldakku");
    ArduinoOTA.setPassword("");
    ArduinoOTA.begin();
}

inline void beginTelnet() {
    TelnetStream.begin();
    TelnetStream.println("Telnet server started");
}

void OTA_CONNECTION::begin() {
    beginWifiAP();
    beginArduinoOTA();
    beginTelnet();
    this->isActive = true;
}

inline void endTelnet() {
    TelnetStream.stop();
}

inline void endArduinoOTA() {
    ArduinoOTA.end();
}

inline void endWifiAP() {
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
}

void OTA_CONNECTION::end() {
    endTelnet();
    endArduinoOTA();
    endWifiAP();
    this->isActive = false;
}

void OTA_CONNECTION::loop() {
    ArduinoOTA.handle();
}

bool OTA_CONNECTION::getIsActive() {
    return this->isActive;
}