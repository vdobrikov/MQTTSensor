#include "MQTTSensor.h"
#include <Arduino.h>

MQTTIntegerSensor::MQTTIntegerSensor(PubSubClient& client, const char* sensorTopic, IntegerUpdateFunction updateFunction) {
  init(client, sensorTopic, updateFunction);
}

MQTTIntegerSensor::MQTTIntegerSensor(PubSubClient& client, const char* sensorTopic, IntegerUpdateFunction updateFunction, uint32_t updatePeriod) {
  init(client, sensorTopic, updateFunction);
  setUpdatePeriod(updatePeriod);
}

void MQTTIntegerSensor::setUpdatePeriod(uint32_t updatePeriod) {
  this->updatePeriod = updatePeriod;
}

void MQTTIntegerSensor::init(PubSubClient& client, const char* sensorTopic, IntegerUpdateFunction updateFunction) {
  this->client = &client;
  this->sensorTopic = sensorTopic;
  this->updateFunction = updateFunction;
}

void MQTTIntegerSensor::loop(){
  runUpdateFunction();
  publishIfNew();
}

void MQTTIntegerSensor::runUpdateFunction() {
  uint32_t currentTimestamp = millis();
  if (currentTimestamp - lastUpdateTimestamp > updatePeriod) {
    Serial.print(sensorTopic);
    Serial.print(" - ");
    Serial.print(currentTimestamp);
    Serial.print(" - ");
    Serial.print(lastUpdateTimestamp);
    Serial.print(" - ");
    Serial.println(updatePeriod);
    if (updateFunction) {
      // Get the value from the update function
      int tmpUpcomingValue = updateFunction();
      // If a valid values was returned, round it, check it's new and then publish it
      if (!isnan(tmpUpcomingValue)) {
        Serial.print("upcomingValue = ");
        Serial.println(tmpUpcomingValue);
	    upcomingValue = tmpUpcomingValue;
        lastUpdateTimestamp = currentTimestamp;
      }
    }
  }
}

void MQTTIntegerSensor::publishIfNew(){
  // Check that the value has changed
  if(upcomingValue != currentValue) {
    const char* payload = getPayloadFrom(upcomingValue);
    Serial.print("Publishing ");
    Serial.print(sensorTopic);
    Serial.print(" ");
    Serial.println(payload);
    client->publish(sensorTopic, payload);
    currentValue = upcomingValue;
  }
}

const char* MQTTIntegerSensor::getPayloadFrom(int newValue) {
  itoa(newValue, payloadBuf, 10);
  return payloadBuf;
}




MQTTBinarySensor::MQTTBinarySensor(PubSubClient& client, const char* sensorTopic, BoolUpdateFunction updateFunction) {
  init(client, sensorTopic, updateFunction);
}

MQTTBinarySensor::MQTTBinarySensor(PubSubClient& client, const char* sensorTopic, BoolUpdateFunction updateFunction, const char* payloadTrue, const char* payloadFalse) {
  init(client, sensorTopic, updateFunction);
  setPayload(payloadTrue, payloadFalse);
}

void MQTTBinarySensor::setPayload(const char* payloadTrue, const char* payloadFalse) {
  this->payloadTrue = payloadTrue;
  this->payloadFalse = payloadFalse;
}

void MQTTBinarySensor::init(PubSubClient& client, const char* sensorTopic, BoolUpdateFunction updateFunction) {
  this->client = &client;
  this->sensorTopic = sensorTopic;
  this->updateFunction = updateFunction;
}

void MQTTBinarySensor::loop(){
  runUpdateFunction();
  publishIfNew();
}

void MQTTBinarySensor::runUpdateFunction() {
  if (updateFunction) {
    upcomingValue = updateFunction();
  }
}

void MQTTBinarySensor::publishIfNew(){
  // Check that the value has changed
  if(upcomingValue != currentValue) {
    const char* payload = getPayloadFrom(upcomingValue);
    client->publish(sensorTopic, payload);
    currentValue = upcomingValue;
  }
}

const char* MQTTBinarySensor::getPayloadFrom(bool newState) {
  if (newState) {
    return payloadTrue;
  } else {
    return payloadFalse;
  }
}
