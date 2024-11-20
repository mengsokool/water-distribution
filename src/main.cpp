#include "header.h"

// Define and initialize the const variables
const char *ssid = "catnip";
const char *password = "1212312121";
const char *mqttHost = "35.197.134.127";
const int mqttPort = 1883; // Initialize the const int here
const char *mqttUser = "powerhub-canteen";
const char *mqttPassword = "powerhub-canteen";

// int K0 = 4;
// int K1 = 0;
// int K2 = 2;
// int K3 = 15;

int modeSelectorSwitch = 5;
int motorOverloadDetector = 15;

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
LiquidCrystal_I2C lcd(0x27, 20, 4);

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

  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");

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
  pinMode(motorOverloadDetector, INPUT_PULLUP);
  publishMessage(topicMotorStatus, "OFF");

  lcd.setCursor(0, 3);
  lcd.printf("MODE: %s", defaultMode);
}

void loop()
{
  unsigned long currentMillis = millis(); // Get the current time in milliseconds

  // Check if 1 second has passed since the last read
  if (currentMillis - previousPzemReadMillis >= pzemReadInterval)
  {
    previousPzemReadMillis = currentMillis;
    readElectricalDataFromPZEM();

    // Show Phase A Data on LCD
    lcd.setCursor(0, 0);
    lcd.printf("%s: %.0f V  %.0f A  %.0f Hz", "A", voltage[0], current[0], frequency[0]);

    sendElectricalDataToCloud(A, VOLTAGE, voltage[0]);
    sendElectricalDataToCloud(A, CURRENT, current[0]);
    sendElectricalDataToCloud(A, POWER, power[0]);
    sendElectricalDataToCloud(A, FREQUENCY, frequency[0]);
    sendElectricalDataToCloud(A, POWER_FACTOR, powerFactor[0]);

    // Show Phase B Data on LCD
    lcd.setCursor(0, 1);
    lcd.printf("%s: %.0f V  %.0f A  %.0f Hz", "B", voltage[1], current[1], frequency[1]);

    sendElectricalDataToCloud(B, VOLTAGE, voltage[1]);
    sendElectricalDataToCloud(B, CURRENT, current[1]);
    sendElectricalDataToCloud(B, POWER, power[1]);
    sendElectricalDataToCloud(B, FREQUENCY, frequency[1]);
    sendElectricalDataToCloud(B, POWER_FACTOR, powerFactor[1]);

    // Show Phase C Data on LCD
    lcd.setCursor(0, 2);
    lcd.printf("%s: %.0f V  %.0f A  %.0f Hz", "C", voltage[2], current[2], frequency[2]);

    sendElectricalDataToCloud(C, VOLTAGE, voltage[2]);
    sendElectricalDataToCloud(C, CURRENT, current[2]);
    sendElectricalDataToCloud(C, POWER, power[2]);
    sendElectricalDataToCloud(C, FREQUENCY, frequency[2]);
    sendElectricalDataToCloud(C, POWER_FACTOR, powerFactor[2]);
  }

  // Handle MQTT messages and other tasks if needed

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

  if (digitalRead(motorOverloadDetector) == LOW)
  {
    motorState = MOTOR_OVERLOAD;
  }
  else
  {
    if (motorState == MOTOR_OVERLOAD)
    {
      lcd.clear();
      lcd.setCursor(0, 3);
      lcd.printf("MODE: %s", defaultMode);
      motorState = MOTOR_STOPPED;
      publishMessage(topicMotorStatus, "OFF");
      if (defaultMode == "APPLICATION")
      {
        onRelay(1);
        onRelay(2);
        onRelay(3);
        onRelay(4);
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
      lcd.clear();
      lcd.setCursor(0, 3);
      lcd.print("MODE: APPLICATION");
    }

    if (digitalRead(modeSelectorSwitch) == HIGH && defaultMode == "APPLICATION")
    {
      emitModeToCloud("MANUAL");
      defaultMode = "MANUAL";
      // digitalWrite(K0, LOW);
      // offRelay(1);
      // offRelay(2);
      // offRelay(3);
      // offRelay(4);
      offRelay(9);
      Serial.println("Change mode to manual");
      lcd.clear();
      lcd.setCursor(0, 3);
      lcd.print("MODE: MANUAL");
    }
  }

  controlMotor();
  mqttClient.loop();
}