#include "MQTTSensor.h"
#include <Arduino.h>

MQTTIntegerSensor::MQTTIntegerSensor(const char* sensorTopic, IntegerUpdateFunction updateFunction, uint32_t updatePeriod) {
  init(sensorTopic, updateFunction);
  setUpdatePeriod(updatePeriod);
}

MQTTIntegerSensor::MQTTIntegerSensor(PubSubClient* client, const char* sensorTopic, IntegerUpdateFunction updateFunction) {
  init(sensorTopic, updateFunction);
  setMqttClient(client);
}

MQTTIntegerSensor::MQTTIntegerSensor(PubSubClient* client, const char* sensorTopic, IntegerUpdateFunction updateFunction, uint32_t updatePeriod) {
  init(sensorTopic, updateFunction);
  setMqttClient(client);
  setUpdatePeriod(updatePeriod);
}

void MQTTIntegerSensor::setMqttClient(PubSubClient* client) {
  this->client = client;
  MQTTSENSOR_PRINT("MQTT sensor: Setting MQTT client. isConnected = ");
  MQTTSENSOR_PRINTLN(this->client->connected());
}

void MQTTIntegerSensor::setUpdatePeriod(uint32_t updatePeriod) {
  this->updatePeriod = updatePeriod;
}

void MQTTIntegerSensor::init(const char* sensorTopic, IntegerUpdateFunction updateFunction) {
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
    MQTTSENSOR_PRINT(sensorTopic);
    MQTTSENSOR_PRINT(" - ");
    MQTTSENSOR_PRINT(currentTimestamp);
    MQTTSENSOR_PRINT(" - ");
    MQTTSENSOR_PRINT(lastUpdateTimestamp);
    MQTTSENSOR_PRINT(" - ");
    MQTTSENSOR_PRINTLN(updatePeriod);
    if (updateFunction) {
      // Get the value from the update function
      int tmpUpcomingValue = updateFunction();
      // If a valid values was returned, check it's new and then publish it
      if (tmpUpcomingValue != NULL) {
        MQTTSENSOR_PRINT("upcomingValue = ");
        MQTTSENSOR_PRINTLN(tmpUpcomingValue);
	    upcomingValue = tmpUpcomingValue;
        lastUpdateTimestamp = currentTimestamp;
      } else {
        MQTTSENSOR_PRINTLN("Skipping NULL upcoming value");
      }
    }
  }
}

void MQTTIntegerSensor::publishIfNew() {
  // Check that the value has changed
  if(upcomingValue != currentValue) {
    const char* payload = getPayloadFrom(upcomingValue);
    MQTTSENSOR_PRINT("Publishing ");
    MQTTSENSOR_PRINT(sensorTopic);
    MQTTSENSOR_PRINT(" ");
    MQTTSENSOR_PRINTLN(payload);
    client->publish(sensorTopic, payload);
    currentValue = upcomingValue;
  }
}

const char* MQTTIntegerSensor::getPayloadFrom(int newValue) {
  itoa(newValue, payloadBuf, 10);
  return payloadBuf;
}




MQTTBinarySensor::MQTTBinarySensor(const char* sensorTopic, BoolUpdateFunction updateFunction) {
  init(sensorTopic, updateFunction);
}

MQTTBinarySensor::MQTTBinarySensor(PubSubClient* client, const char* sensorTopic, BoolUpdateFunction updateFunction) {
  init(sensorTopic, updateFunction);
  setMqttClient(client);
}

MQTTBinarySensor::MQTTBinarySensor(const char* sensorTopic, BoolUpdateFunction updateFunction, const char* payloadTrue, const char* payloadFalse) {
  init(sensorTopic, updateFunction);
  setPayload(payloadTrue, payloadFalse);
}

MQTTBinarySensor::MQTTBinarySensor(PubSubClient* client, const char* sensorTopic, BoolUpdateFunction updateFunction, const char* payloadTrue, const char* payloadFalse) {
  init(sensorTopic, updateFunction);
  setMqttClient(client);
  setPayload(payloadTrue, payloadFalse);
}

void MQTTBinarySensor::setMqttClient(PubSubClient* client) {
  this->client = client;
}

void MQTTBinarySensor::setPayload(const char* payloadTrue, const char* payloadFalse) {
  this->payloadTrue = payloadTrue;
  this->payloadFalse = payloadFalse;
}

void MQTTBinarySensor::init(const char* sensorTopic, BoolUpdateFunction updateFunction) {
  this->sensorTopic = sensorTopic;
  this->updateFunction = updateFunction;
}

void MQTTBinarySensor::loop() {
  runUpdateFunction();
  publishIfNew();
}

void MQTTBinarySensor::runUpdateFunction() {
  if (updateFunction) {
    upcomingValue = updateFunction();
  }
}

void MQTTBinarySensor::publishIfNew() {
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
