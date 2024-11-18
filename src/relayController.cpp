#include <Arduino.h>

#define RS485_RX_PIN 19
#define RS485_TX_PIN 18
#define RS485_CONTROL_PIN 4
#define RS485_TRANSMIT HIGH
#define RS485_RECEIVE LOW

#define DELAY_TIME 100

#define COMMAND_LENGTH_8 8
#define COMMAND_LENGTH_10 10

HardwareSerial RS485Serial(1); // Use hardware serial port 2

byte DeviceID = 0x01;

// Relay control commands
byte ON_RELAY_1[COMMAND_LENGTH_8] = {DeviceID, 0x05, 0x00, 0x00, 0xFF, 0x00, 0x8C, 0x3A};
byte OFF_RELAY_1[COMMAND_LENGTH_8] = {DeviceID, 0x05, 0x00, 0x00, 0x00, 0x00, 0xCD, 0xCA};

byte ON_RELAY_2[COMMAND_LENGTH_8] = {DeviceID, 0x05, 0x00, 0x01, 0xFF, 0x00, 0xDD, 0xFA};
byte OFF_RELAY_2[COMMAND_LENGTH_8] = {DeviceID, 0x05, 0x00, 0x01, 0x00, 0x00, 0x9C, 0x0A};

byte ON_RELAY_3[COMMAND_LENGTH_8] = {DeviceID, 0x05, 0x00, 0x02, 0xFF, 0x00, 0x2D, 0xFA};
byte OFF_RELAY_3[COMMAND_LENGTH_8] = {DeviceID, 0x05, 0x00, 0x02, 0x00, 0x00, 0x6C, 0x0A};

byte ON_RELAY_4[COMMAND_LENGTH_8] = {DeviceID, 0x05, 0x00, 0x03, 0xFF, 0x00, 0x7C, 0x3A};
byte OFF_RELAY_4[COMMAND_LENGTH_8] = {DeviceID, 0x05, 0x00, 0x03, 0x00, 0x00, 0x3D, 0xCA};

byte ON_RELAY_5[COMMAND_LENGTH_8] = {DeviceID, 0x05, 0x00, 0x04, 0xFF, 0x00, 0xCD, 0xFB};
byte OFF_RELAY_5[COMMAND_LENGTH_8] = {DeviceID, 0x05, 0x00, 0x04, 0x00, 0x00, 0x8C, 0x0B};

byte ON_RELAY_6[COMMAND_LENGTH_8] = {DeviceID, 0x05, 0x00, 0x05, 0xFF, 0x00, 0x9C, 0x3B};
byte OFF_RELAY_6[COMMAND_LENGTH_8] = {DeviceID, 0x05, 0x00, 0x05, 0x00, 0x00, 0xDD, 0xCB};

byte ON_RELAY_7[COMMAND_LENGTH_8] = {DeviceID, 0x05, 0x00, 0x06, 0xFF, 0x00, 0x6C, 0x3B};
byte OFF_RELAY_7[COMMAND_LENGTH_8] = {DeviceID, 0x05, 0x00, 0x06, 0x00, 0x00, 0x2D, 0xCB};

byte ON_RELAY_8[COMMAND_LENGTH_8] = {DeviceID, 0x05, 0x00, 0x07, 0xFF, 0x00, 0x3D, 0xFB};
byte OFF_RELAY_8[COMMAND_LENGTH_8] = {DeviceID, 0x05, 0x00, 0x07, 0x00, 0x00, 0x7C, 0x0B};

byte ON_RELAY_ALL[COMMAND_LENGTH_10] = {DeviceID, 0x0F, 0x00, 0x00, 0x00, 0x08, 0x01, 0xFF, 0xBE, 0xD5};
byte OFF_RELAY_ALL[COMMAND_LENGTH_10] = {DeviceID, 0x0F, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0xFE, 0x95};

void initRelayControl()
{
    pinMode(RS485_CONTROL_PIN, OUTPUT);
    digitalWrite(RS485_CONTROL_PIN, RS485_RECEIVE);
    // Initialize hardware serial port 2 with the correct pins
    RS485Serial.begin(9600, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);
}

void sendCommand(byte *command, int length)
{
    digitalWrite(RS485_CONTROL_PIN, RS485_TRANSMIT);
    RS485Serial.write(command, length);
    delay(DELAY_TIME);
}

void switchToReceiveMode()
{
    digitalWrite(RS485_CONTROL_PIN, RS485_RECEIVE);
}

void onRelay(int relay)
{
    switch (relay)
    {
    case 1:
        sendCommand(ON_RELAY_1, COMMAND_LENGTH_8);
        break;
    case 2:
        sendCommand(ON_RELAY_2, COMMAND_LENGTH_8);
        break;
    case 3:
        sendCommand(ON_RELAY_3, COMMAND_LENGTH_8);
        break;
    case 4:
        sendCommand(ON_RELAY_4, COMMAND_LENGTH_8);
        break;
    case 5:
        sendCommand(ON_RELAY_5, COMMAND_LENGTH_8);
        break;
    case 6:
        sendCommand(ON_RELAY_6, COMMAND_LENGTH_8);
        break;
    case 7:
        sendCommand(ON_RELAY_7, COMMAND_LENGTH_8);
        break;
    case 8:
        sendCommand(ON_RELAY_8, COMMAND_LENGTH_8);
        break;
    case 9:
        sendCommand(ON_RELAY_ALL, COMMAND_LENGTH_10);
        break;
    default:
        break;
    }
}

void offRelay(int relay)
{
    switch (relay)
    {
    case 1:
        sendCommand(OFF_RELAY_1, COMMAND_LENGTH_8);
        break;
    case 2:
        sendCommand(OFF_RELAY_2, COMMAND_LENGTH_8);
        break;
    case 3:
        sendCommand(OFF_RELAY_3, COMMAND_LENGTH_8);
        break;
    case 4:
        sendCommand(OFF_RELAY_4, COMMAND_LENGTH_8);
        break;
    case 5:
        sendCommand(OFF_RELAY_5, COMMAND_LENGTH_8);
        break;
    case 6:
        sendCommand(OFF_RELAY_6, COMMAND_LENGTH_8);
        break;
    case 7:
        sendCommand(OFF_RELAY_7, COMMAND_LENGTH_8);
        break;
    case 8:
        sendCommand(OFF_RELAY_8, COMMAND_LENGTH_8);
        break;
    case 9:
        sendCommand(OFF_RELAY_ALL, COMMAND_LENGTH_10);
        break;
    default:
        break;
    }
}