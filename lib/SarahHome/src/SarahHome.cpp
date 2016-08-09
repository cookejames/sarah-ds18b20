#include "SarahHome.h"

KeyValueStore kvstore;

SarahHome::SarahHome() {
  mqttClientNameFormat = "sarah-%d";

  mqttUsername = kvstore.read("mqttUsername");
  mqttPassword = kvstore.read("mqttPassword");
  mqttServer = kvstore.read("mqttServer");

  wifiSsid = kvstore.read("wifiSsid");
  wifiPassword = kvstore.read("wifiPassword");

  nodeId = kvstore.read("nodeId");
}

void SarahHome::setup() {
  connectWifi();
  setMqttName();
  connectMqtt();
  setupOTA();
  Serial.printf("Node %s ready\n", nodeId);
}

void SarahHome::setupOTA() {
  ArduinoOTA.setHostname(nodeId);

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%", (progress / (total / 100)));
    Serial.println();
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void SarahHome::connectWifi() {
  WiFi.mode(WIFI_STA);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf("Attempting to connect to WPA SSID: %s\n", wifiSsid);
    // Connect to WPA/WPA2 network:
    WiFi.begin(wifiSsid, wifiPassword);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println("**WiFi connected**");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("Signal strength: %s\n", WiFi.RSSI());
}

void SarahHome::setMqttName() {
  randomSeed(RANDOM_REG32);
  sprintf(mqttClientName, mqttClientNameFormat, random(1000));
}

void SarahHome::connectMqtt() {
  mqttClient = PubSubClient(wifiClient);
  mqttClient.setServer(mqttServer, 1883);

  int attempts = 0;
  while (!mqttClient.connected()) {
    Serial.printf("MQTT client name: %s\n", mqttClientName);
    Serial.printf("Attempting to connect to MQTT server: %s\n", mqttServer);
    boolean connected = mqttClient.connect(
      mqttClientName,
      mqttUsername,
      mqttPassword,
      "devices/disconnected",
      0,
      0,
      nodeId
    );
    if (!connected) {
      Serial.print("MQTT Connection Error: ");
      Serial.println(mqttClient.state());

      attempts++;
      if (attempts >=5) {
        Serial.println("Failed to connnected to MQTT after 5 attempts, restarting...");
        ESP.restart();
      }
      delay(10000);
    }
  }

  Serial.println("**MQTT Connected**");
  mqttClient.publish("devices/connected", nodeId);
}

void SarahHome::loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }
  ArduinoOTA.handle();
  if (!mqttClient.loop()) {
    connectMqtt();
  }
}
