#include "header.h"

// Define and initialize the const variables
const char *ssid = "";
const char *password = "";
const char *mqttHost = "";
const int mqttPort = 1883; // Initialize the const int here
const char *mqttUser = "";
const char *mqttPassword = "";

// int K0 = 4;
// int K1 = 0;
// int K2 = 2;
// int K3 = 15;

int modeSelectorSwitch = 5;

const char *topicMotorStatus = "motor_status";
const char *topicSetMotorFromServer = "set_motor_from_server";
const char *topicSetMotorFromHardware = "set_motor_from_hardware";
const char *topicSetModeFromHardware = "set_mode_from_hardware";
const char *electricalTopic = "electrical";

char *defaultMode = "MANUAL";
MotorState motorState = MOTOR_STOPPED;

// Initialize the WiFiClient and PubSubClient
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
PZEM004Tv30 pzems[3];

// Memory initialization
float voltage[3] = {0, 0, 0};
float current[3] = {0, 0, 0};
float power[3] = {0, 0, 0};
float frequency[3] = {0, 0, 0};
float powerFactor[3] = {0, 0, 0};

// Timer
unsigned long previousPzemReadMillis = 0;
const long pzemReadInterval = 1000;

void setup()
{
  // Initialize Hardware
  Serial.begin(115200);
  initPZEM();

  // Initialize Wi-Fi
  connectToWiFi();

  // Initialize MQTT client
  connectToMQTT();

  // pinMode(K0, OUTPUT); // Relay 1
  // pinMode(K1, OUTPUT); // Relay 2
  // pinMode(K2, OUTPUT); // Relay 3
  // pinMode(K3, OUTPUT); // Relay 4
  pinMode(RS485_CONTROL_PIN, OUTPUT);
  digitalWrite(RS485_CONTROL_PIN, RS485_RECEIVE);

  initRelayControl();
  offRelay(9);

  pinMode(modeSelectorSwitch, INPUT);
  publishMessage(topicMotorStatus, "OFF");
}

void loop()
{
  unsigned long currentMillis = millis(); // Get the current time in milliseconds

  // Check if 1 second has passed since the last read
  if (currentMillis - previousPzemReadMillis >= pzemReadInterval)
  {
    previousPzemReadMillis = currentMillis;
    readElectricalDataFromPZEM();

    sendElectricalDataToCloud(A, VOLTAGE, voltage[0]);
    sendElectricalDataToCloud(A, CURRENT, current[0]);
    sendElectricalDataToCloud(A, POWER, power[0]);
    sendElectricalDataToCloud(A, FREQUENCY, frequency[0]);
    sendElectricalDataToCloud(A, POWER_FACTOR, powerFactor[0]);

    sendElectricalDataToCloud(B, VOLTAGE, voltage[1]);
    sendElectricalDataToCloud(B, CURRENT, current[1]);
    sendElectricalDataToCloud(B, POWER, power[1]);
    sendElectricalDataToCloud(B, FREQUENCY, frequency[1]);
    sendElectricalDataToCloud(B, POWER_FACTOR, powerFactor[1]);

    sendElectricalDataToCloud(C, VOLTAGE, voltage[2]);
    sendElectricalDataToCloud(C, CURRENT, current[2]);
    sendElectricalDataToCloud(C, POWER, power[2]);
    sendElectricalDataToCloud(C, FREQUENCY, frequency[2]);
    sendElectricalDataToCloud(C, POWER_FACTOR, powerFactor[2]);
  }

  // Handle MQTT messages and other tasks if needed
  mqttClient.loop();

  if (!mqttClient.connected())
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      offRelay(9);
      ESP.restart();
    }
    else
    {
      connectToMQTT();
    }
  }

  if (digitalRead(modeSelectorSwitch) == LOW && defaultMode == "MANUAL")
  {
    emitModeToCloud("APPLICATION");
    defaultMode = "APPLICATION";
    // digitalWrite(K0, HIGH);
    onRelay(1);
    onRelay(2);
    onRelay(3);
    onRelay(4);
    Serial.println("Change mode to application");
  }

  if (digitalRead(modeSelectorSwitch) == HIGH && defaultMode == "APPLICATION")
  {
    emitModeToCloud("MANUAL");
    defaultMode = "MANUAL";
    // digitalWrite(K0, LOW);
    offRelay(1);
    offRelay(2);
    offRelay(3);
    offRelay(4);
    Serial.println("Change mode to manual");
  }

  controlMotor();
}