#include "header.h"

// Function to connect to Wi-Fi
void connectToWiFi()
{
  Serial.println("Connecting to Wi-Fi...");
  lcd.setCursor(0, 1);
  lcd.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nWi-Fi connected.");
  lcd.setCursor(0, 1);
  lcd.print("                  ");
  lcd.print("Wi-Fi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void handleMQTTMessage(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  Serial.print("Message: ");

  // สร้าง buffer พร้อมใส่ null terminator
  char message[length + 1];
  for (unsigned int i = 0; i < length; i++)
  {
    message[i] = (char)payload[i];
  }
  message[length] = '\0'; // เพิ่ม null terminator

  Serial.println(message); // พิมพ์ข้อความออกมา

  // I want some logic to compare message between "ON" or "OFF"
  if (strcmp(message, "ON") == 0)
  {
    startMotor();
  }
  else if (strcmp(message, "OFF") == 0)
  {
    stopMotor();
  }
}

void emitModeToCloud(char *mode)
{
  if (mqttClient.connected())
  {
    publishMessage(topicSetModeFromHardware, mode);
  }
}

// Function to connect to MQTT broker
void connectToMQTT()
{
  while (!mqttClient.connected())
  {
    mqttClient.setServer(mqttHost, mqttPort);
    mqttClient.setCallback(handleMQTTMessage);
    Serial.print("Connecting to MQTT broker...");
    lcd.setCursor(0, 1);
    lcd.print("                 ");
    lcd.print("Connecting to MQ");
    if (mqttClient.connect("ESP32Client", mqttUser, mqttPassword))
    {
      Serial.println("connected.");
      mqttClient.subscribe("set_motor_from_server"); // Subscribe to a topic
      lcd.setCursor(0, 0);
      lcd.clear();
    }
    else
    {
      Serial.print("Failed. Error code: ");
      lcd.setCursor(0, 1);
      lcd.print("                  ");
      lcd.print("MQTT Conn. Failed");
      lcd.setCursor(0, 2);
      lcd.print("                  ");
      lcd.print("Retrying in 5 sec.");
      Serial.println(mqttClient.state());
      Serial.println("Retrying in 5 seconds...");
      delay(5000);
      lcd.setCursor(0, 2);
      lcd.print("                  ");
    }
  }
}

// Function to publish a message to a topic
void publishMessage(const char *topic, const char *message)
{
  if (mqttClient.publish(topic, message))
  {
    // Serial.print("Message published to topic ");
    // Serial.println(topic);
  }
  else
  {
    Serial.print("Message failed to publish : ");
    Serial.println(mqttClient.connected() ? "MQTT CONNECTED" : "MQTT DISCONNECTED");
  }
}