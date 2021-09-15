#pragma once

// This is a very simple class that just keeps track of what pins a motor uses and has some functions to update speed and direction.
class Motor {
public:
  Motor(unsigned int pinA, unsigned int pinB, unsigned int pwmPin) {
    this->pinA = pinA;
    this->pinB = pinB;
    this->pwmPin = pwmPin;

    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
  }

  // Set the speed of the motor between 0 and 255 for 0% and 100% throttle.
  void setSpeed(unsigned char speed) {
    analogWrite(pwmPin, speed);
  }

  // Set the direction of the motor.
  // true: forwards
  // false: backwards
  void setDirection(bool dir) {
    if(dir) {
      digitalWrite(pinA, LOW);
      digitalWrite(pinB, HIGH);
    } else {
      digitalWrite(pinA, HIGH);
      digitalWrite(pinB, LOW);
    }
  }

private:
  unsigned int pinA, pinB, pwmPin;
};