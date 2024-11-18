#include "header.h"

void initPZEM()
{
    pzems[0] = PZEM004Tv30(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x1);
    pzems[1] = PZEM004Tv30(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x7);
    pzems[2] = PZEM004Tv30(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x10);
}

void readElectricalDataFromPZEM()
{
    for (int i = 0; i < sizeof(pzems) / sizeof(pzems[0]); i++)
    {
        voltage[i] = pzems[i].voltage();
        current[i] = pzems[i].current();
        power[i] = pzems[i].power();
        frequency[i] = pzems[i].frequency();
        powerFactor[i] = pzems[i].pf();
    }
}

void sendElectricalDataToCloud(Phase phase, ElectricalType type, float value)
{
    JsonDocument doc;
    JsonArray data = doc.to<JsonArray>();

    // แยก switch ออกมาเป็นตัวแปรก่อน
    const char *phaseStr;
    switch (phase)
    {
    case A:
        phaseStr = "A";
        break;
    case B:
        phaseStr = "B";
        break;
    case C:
        phaseStr = "C";
        break;
    default:
        phaseStr = "UNKNOWN";
        break;
    }

    const char *typeStr;
    switch (type)
    {
    case VOLTAGE:
        typeStr = "VOLTAGE";
        break;
    case CURRENT:
        typeStr = "CURRENT";
        break;
    case POWER:
        typeStr = "POWER";
        break;
    case FREQUENCY:
        typeStr = "FREQUENCY";
        break;
    case POWER_FACTOR:
        typeStr = "PF";
        break;
    default:
        typeStr = "UNKNOWN";
        break;
    }

    data.add(phaseStr);
    data.add(typeStr);
    data.add(value);

    char buffer[64];
    serializeJson(data, buffer);
    publishMessage(electricalTopic, buffer);
}