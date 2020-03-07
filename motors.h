#pragma once

#define MOTOR_MIN 50

class Motors {
public:
    // static const int fwd = 5;
    // static const int rev = 6;
    static const int fwd = 4;
    static const int rev = 3;

    // static const int enable = 23;
    static const int enable = 22;

    void Attach();
    void Stop();
    // Ranges -255 ... 255
    void SetSpeeds(int speed);
};