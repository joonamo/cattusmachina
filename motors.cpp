#include "motors.h"
#include <Arduino.h>
#include <HardwareSerial.h>

void Motors::Attach() {
    pinMode(fwd, OUTPUT);
    pinMode(rev, OUTPUT);
    pinMode(enable, OUTPUT);

    Stop();
}

void Motors::Stop() {
    analogWrite(enable, 0);
}

void Motors::SetSpeeds(int speed) {
    if (speed == 0)
    {
        digitalWrite(enable, 0);
        digitalWrite(fwd, LOW);
        digitalWrite(rev, LOW);
    }
    else
    {
        if (speed > 0) 
        {
            digitalWrite(fwd, LOW);
            digitalWrite(rev, HIGH);
        } 
        else 
        {
            digitalWrite(fwd, HIGH);
            digitalWrite(rev, LOW);
        }
        int v = abs(speed) + MOTOR_MIN;
        if (v > 255) v = 255;
        analogWrite(enable, v);
    }
}