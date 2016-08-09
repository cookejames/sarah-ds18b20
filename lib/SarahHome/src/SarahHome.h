#ifndef SarahHome_h
#define SarahHome_h

#include "ESP8266WiFi.h"
#include "ESP8266mDNS.h"
#include "WiFiUdp.h"
#include "ArduinoOTA.h"
#include "PubSubClient.h"

class SarahHome
{
  public:
    SarahHome();
    void setup();
    void loop();

    PubSubClient mqttClient;
  private:
    void setMqttName();
    void connectWifi();
    void connectMqtt();
    void setupOTA();

    const char* mqttUsername;
    const char* mqttPassword;
    const char* mqttServer;
    const char* mqttClientNameFormat;
    char mqttClientName[20];
    WiFiClient wifiClient;

    const char* wifiSsid;
    const char* wifiPassword;

    const char* nodeId;
};
#endif
