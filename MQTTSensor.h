#ifndef MQTTSensor_H
#define MQTTSensor_H

#include <arduino.h>
#include <functional>
#include "PubSubClient.h"

// Forward definition
class MQTTIntegerSensor;
class MQTTBinarySensor;

#define PAYLOAD_STRING_MAX_LENGTH 10
#define DEFAULT_UPDATE_PERIOD 1000
#define DEFAULT_PAYLOAD_TRUE "ON"
#define DEFAULT_PAYLOAD_FALSE "OFF"

// Define callback types
typedef std::function<int(void)> IntegerUpdateFunction;
typedef std::function<bool(void)> BoolUpdateFunction;

// Class definition
class MQTTIntegerSensor {
  private:
    PubSubClient* client;
    const char* sensorTopic;
    char payloadBuf[PAYLOAD_STRING_MAX_LENGTH];
    int currentValue;
    int upcomingValue;

    IntegerUpdateFunction updateFunction;
    uint32_t updatePeriod = DEFAULT_UPDATE_PERIOD;
    uint32_t lastUpdateTimestamp;

    void init(PubSubClient&, const char*, IntegerUpdateFunction);
    const char* getPayloadFrom(int);
    void runUpdateFunction();
    void publishIfNew();

  public:
    MQTTIntegerSensor(PubSubClient&, const char*, IntegerUpdateFunction);
    MQTTIntegerSensor(PubSubClient&, const char*, IntegerUpdateFunction, uint32_t);
    void setUpdatePeriod(uint32_t);
    void loop();
};


class MQTTBinarySensor {
  private:
    PubSubClient* client;
    const char* sensorTopic;
    bool currentValue;
    bool upcomingValue;

    const char* payloadTrue = DEFAULT_PAYLOAD_TRUE;
    const char* payloadFalse = DEFAULT_PAYLOAD_FALSE;
    BoolUpdateFunction updateFunction;

    const char* getPayloadFrom(bool);
    void init(PubSubClient&, const char*, BoolUpdateFunction);
    void runUpdateFunction();
    void publishIfNew();

  public:
    MQTTBinarySensor(PubSubClient&, const char*, BoolUpdateFunction);
    MQTTBinarySensor(PubSubClient&, const char*, BoolUpdateFunction, const char*, const char*);
    void setPayload(const char*, const char*);
    void loop();
};

#endif // MQTTSensor_H
