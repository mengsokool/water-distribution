#include "header.h"

unsigned long lastMillis = 0;
unsigned long interval = 0;
bool deltaMessageSent = false; // ตัวแปรสำหรับตรวจสอบสถานะการส่งข้อความ

void controlMotor()
{
    switch (motorState)
    {
    case MOTOR_STOPPED:
        // หยุดมอเตอร์ทั้งหมด
        // digitalWrite(K1, LOW);
        // digitalWrite(K2, LOW);
        // digitalWrite(K3, LOW);
        offRelay(6);
        offRelay(7);
        offRelay(8);
        deltaMessageSent = false; // รีเซ็ต flag เมื่อหยุดมอเตอร์
        break;

    case MOTOR_STAR:
        // เริ่มในโหมด Star
        // digitalWrite(K1, HIGH);  // เปิดรีเลย์ Main
        onRelay(8);
        onRelay(6);
        // digitalWrite(K2, HIGH); // เปิดรีเลย์ Star
        Serial.println("Motor started in Star mode");
        publishMessage(topicMotorStatus, "STAR");

        // ตั้งเวลาเพื่อเปลี่ยนไปยังโหมด Delta
        lastMillis = millis();
        interval = 5000; // รอ 5 วินาทีในโหมด Star
        motorState = MOTOR_WAITING;
        break;

    case MOTOR_WAITING:
        if (millis() - lastMillis >= interval)
        {
            // ปิดรีเลย์ Star และเตรียมรอ 250ms ก่อนเข้าสู่โหมด Delta
            // digitalWrite(K2, LOW);
            offRelay(6);
            Serial.println("Waiting before switching to Delta mode");
            lastMillis = millis(); // ตั้งเวลาใหม่สำหรับรอ 250ms
            interval = 250;
            motorState = MOTOR_WAITING_DELTA;
        }
        break;

    case MOTOR_WAITING_DELTA:
        if (millis() - lastMillis >= interval)
        {
            // เตรียมเข้าสู่โหมด Delta
            motorState = MOTOR_DELTA;
        }
        break;

    case MOTOR_DELTA:
        if (!deltaMessageSent)
        { // ตรวจสอบว่าข้อความ `DELTA` ถูกส่งแล้วหรือยัง
            // digitalWrite(K3, HIGH); // เปิดรีเลย์ Delta
            onRelay(7);
            Serial.println("Motor switched to Delta mode");
            publishMessage(topicMotorStatus, "DELTA");
            deltaMessageSent = true; // ตั้งค่า flag เพื่อป้องกันการส่งซ้ำ
        }
        break;

    default:
        break;
    }
}

void startMotor()
{
    if (motorState == MOTOR_STOPPED)
    {
        motorState = MOTOR_STAR; // เริ่มจากโหมด Star
    }
}

void stopMotor()
{
    motorState = MOTOR_STOPPED; // หยุดมอเตอร์
    Serial.println("Motor stopped");
    publishMessage(topicMotorStatus, "OFF");
}
