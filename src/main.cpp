#include "OneWire.h"
#include "DallasTemperature.h"
#include "SarahHome.h"

const char* mqttTopic = "sensors/56ec57b951b33c384fbc2f6e/readings";

//Temperature sensor
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
SarahHome sarahHome;

float lastTemperature;
unsigned long lastTemperaturePublish = 0;
unsigned long timeBetweenPublish = 300000; //5 minutes

void setup()
{
  Serial.begin(9600);
  sarahHome.setup();
}

bool publishValue(float v, const char* topic) {
  if (isnan(v)) {
    return false;
  }
  String value;
  value += v;
  Serial.print("Sending to " + (String)mqttTopic + ": ");
  Serial.println(value);
  return sarahHome.mqttClient.publish(topic, value.c_str());
}

void publishTemperature() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  float temperature = sensors.getTempCByIndex(0);
  if ((millis() - lastTemperaturePublish) > timeBetweenPublish ||
      (temperature > lastTemperature + 0.5) || (temperature < lastTemperature - 0.5)) {
    if (publishValue(temperature, mqttTopic)) {
        lastTemperature = temperature;
        lastTemperaturePublish = millis();
    }
  }
}

void loop() {
  sarahHome.loop();

  publishTemperature();

  delay(1000);
}
