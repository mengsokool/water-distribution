#ifndef HEADER_H
#define HEADER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <PZEM004Tv30.h>
#include <HardwareSerial.h>

// ----------------------------
// Network Configurations
// ----------------------------
extern const char *ssid;
extern const char *password;
extern const char *mqttHost;
extern const int mqttPort;
extern const char *mqttUser;
extern const char *mqttPassword;

// ----------------------------
// GPIO Pin Definitions
// ----------------------------
#define PZEM_RX_PIN      16
#define PZEM_TX_PIN      17
#define PZEM_SERIAL      Serial2

#define RS485_RX_PIN     19
#define RS485_TX_PIN     18
#define RS485_CONTROL_PIN 4
#define RS485_TRANSMIT   HIGH
#define RS485_RECEIVE    LOW

// Motor Control GPIO
extern int K1;
extern int K2;
extern int K3;
extern int modeSelectorSwitch;

// ----------------------------
// RS485 and Relay Control
// ----------------------------
extern HardwareSerial RS485Serial;

#define COMMAND_LENGTH_8  8
#define COMMAND_LENGTH_10 10

extern byte DeviceID;

// Relay control commands
extern byte ON_RELAY_1[COMMAND_LENGTH_8];
extern byte OFF_RELAY_1[COMMAND_LENGTH_8];

extern byte ON_RELAY_2[COMMAND_LENGTH_8];
extern byte OFF_RELAY_2[COMMAND_LENGTH_8];

extern byte ON_RELAY_3[COMMAND_LENGTH_8];
extern byte OFF_RELAY_3[COMMAND_LENGTH_8];

extern byte ON_RELAY_4[COMMAND_LENGTH_8];
extern byte OFF_RELAY_4[COMMAND_LENGTH_8];

extern byte ON_RELAY_5[COMMAND_LENGTH_8];
extern byte OFF_RELAY_5[COMMAND_LENGTH_8];

extern byte ON_RELAY_6[COMMAND_LENGTH_8];
extern byte OFF_RELAY_6[COMMAND_LENGTH_8];

extern byte ON_RELAY_7[COMMAND_LENGTH_8];
extern byte OFF_RELAY_7[COMMAND_LENGTH_8];

extern byte ON_RELAY_8[COMMAND_LENGTH_8];
extern byte OFF_RELAY_8[COMMAND_LENGTH_8];

extern byte ON_RELAY_ALL[COMMAND_LENGTH_10];
extern byte OFF_RELAY_ALL[COMMAND_LENGTH_10];

// ----------------------------
// Global Objects
// ----------------------------
extern WiFiClient wifiClient;
extern PubSubClient mqttClient;
extern PZEM004Tv30 pzems[3];

// ----------------------------
// Enumerations
// ----------------------------
enum Phase {
    A,
    B,
    C
};

enum ElectricalType {
    VOLTAGE,
    CURRENT,
    POWER,
    FREQUENCY,
    POWER_FACTOR
};

enum MotorState {
    MOTOR_STOPPED,
    MOTOR_STAR,
    MOTOR_WAITING,
    MOTOR_WAITING_DELTA,
    MOTOR_DELTA
};

extern MotorState motorState;

// ----------------------------
// MQTT Topics
// ----------------------------
extern const char *topicMotorStatus;
extern const char *topicSetMotorFromServer;
extern const char *topicSetMotorFromHardware;
extern const char *topicSetModeFromHardware;
extern const char *electricalTopic;

// ----------------------------
// Measurement Data
// ----------------------------
extern float voltage[3];
extern float current[3];
extern float power[3];
extern float frequency[3];
extern float powerFactor[3];

// Default mode for operation
extern char *defaultMode;

// ----------------------------
// Timer Variables
// ----------------------------
extern unsigned long previousPzemReadMillis;
extern const long pzemReadInterval;

// ----------------------------
// Hardware Function Declarations
// ----------------------------
void startMotor();
void controlMotor();
void stopMotor();
void readElectricalDataFromPZEM();
void initPZEM();

// ----------------------------
// Cloud Client Function Declarations
// ----------------------------
void sendElectricalDataToCloud(Phase phase, ElectricalType type, float value);
void emitModeToCloud(char *mode);

// ----------------------------
// Network Management Function Declarations
// ----------------------------
void connectToWiFi();
void handleMQTTMessage(char *topic, byte *payload, unsigned int length);
void connectToMQTT();
void publishMessage(const char *topic, const char *message);

// ----------------------------
// Relay Control Function Declarations
// ----------------------------
void initRelayControl();
void sendCommand(byte *command, int length);
void switchToReceiveMode();
void onRelay(int relay);
void offRelay(int relay);

#endif // HEADER_H
