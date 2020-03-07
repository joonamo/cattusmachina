#include <Servo.h>
#include <Metro.h>
//#define Serial Serial1
#include <ResponsiveAnalogRead.h>
#include "Motors.h"
#include <Bounce.h>

#include <stdarg.h>
void p(char *fmt, ... ){
  char buf[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt );
  vsnprintf(buf, 128, fmt, args);
  va_end (args);
  Serial.println(buf);
  Serial1.println(buf);
}

void p(const __FlashStringHelper *fmt, ... ){
  char buf[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt);
#ifdef __AVR__
  vsnprintf_P(buf, sizeof(buf), (const char *)fmt, args); // progmem for AVR
#else
  vsnprintf(buf, sizeof(buf), (const char *)fmt, args); // for the rest of the world
#endif
  va_end(args);
  Serial.println(buf);
  Serial1.println(buf);
}

#define BUMPER_BUTTON 14
Bounce bumper = Bounce(BUMPER_BUTTON, 5); 
#define DIST_IN_L 16
ResponsiveAnalogRead dist_l_reader(DIST_IN_L, true);
int dist_l = 0;
#define DIST_IN_R 15
ResponsiveAnalogRead dist_r_reader(DIST_IN_R, true);
int dist_r = 0;

#define HAND_LEFT 10
#define HAND_LEFT_ZERO 0
#define HAND_LEFT_MAX 180
Servo handLeft;
int handLeftDir = HAND_LEFT_ZERO;

#define HAND_RIGHT 9
#define HAND_RIGHT_ZERO 180
#define HAND_RIGHT_MAX 30
Servo handRight;
int handRightDir = HAND_RIGHT_ZERO;

Metro debug_out = Metro(10000);

Motors motors = Motors();
int speed = 0;
int speedChange = 1;

#define RoboState uint8_t
#define StateInitial 0
#define StateGoingForward 1
#define StateHitWall 2
#define StateTurningLeft 3
#define StateTurningRight 4
RoboState currentState = StateInitial;
int prevTime = 0;
int deltaTime = 0;
int timeInState = 0;

void changeState(RoboState state) {
  currentState = state;
  timeInState = 0;
}

void setup() {                
  // initialize the digital pin as an output.
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  delay(100);
  digitalWrite(13, HIGH);

  handRight.attach(HAND_RIGHT, 450, 2400);
  handLeft.attach(HAND_LEFT, 490, 2520);

  pinMode(BUMPER_BUTTON, INPUT_PULLUP);
  motors.Attach();

  Serial.begin(115200);
}

// the loop routine runs over and over again forever:
void loop() {
  dist_l_reader.update();
  dist_r_reader.update();
  dist_l = dist_l_reader.getValue();
  dist_r = dist_r_reader.getValue();
  bumper.update();
  const int bumperHit = !bumper.read();

  switch (currentState) {
    default:
    case StateInitial: {
      speed = 0;
      if (bumperHit) {
        delay(1000);
        changeState(StateGoingForward);
      }
      break;
    }
    case StateGoingForward: {
      handLeftDir = HAND_LEFT_ZERO;
      handRightDir = HAND_RIGHT_ZERO;
      speed = 255;
      if (bumperHit) {
        speed = 0;
        changeState(dist_l > dist_r ? StateTurningRight : StateTurningLeft);
      }
      break;
    }
    case StateTurningRight: {
      handLeftDir = HAND_LEFT_MAX;
      speed = timeInState < 200 ? -1 : 0;
      if (timeInState > 500) {
        changeState(StateGoingForward);
      }
      break;
    }
    case StateTurningLeft: {
      handRightDir = HAND_RIGHT_MAX;
      speed = timeInState < 200 ? -1 : 0;
      if (timeInState > 500) {
        changeState(StateGoingForward);
      }
      break;
    }
  }

  Serial.println(timeInState);
  handLeft.write(handLeftDir);
  handRight.write(handRightDir);
  motors.SetSpeeds(speed);

  const uint currentTime = millis();
  deltaTime = currentTime - prevTime;
  timeInState += deltaTime;
  prevTime = currentTime;
}