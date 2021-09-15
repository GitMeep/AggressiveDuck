#pragma once
#include "LightSensor.h"
#include "Controller.h"

// A controller that gives commands based on the sensor readings.
class Autonomy : public Controller {
public:

  Autonomy(LightSensor* rightSensor, LightSensor* middleSensor, LightSensor* leftSensor, LightSensor* bottomSensor) {
    this->rightSensor = rightSensor;
    this->leftSensor = leftSensor;
    this->middleSensor = middleSensor;
    this->bottomSensor = bottomSensor;

    this->turn = 0;
  }

  // Each time this controller is updated, it reads the sensors. If at least one sees black it decides which direction to go. If none see black, it continues in the last direction it had.
  // TODO: Enter search mode if bottom sensor is not reading black.
  ControlCommand update() {
    ControlCommand command;
    // Read all the sensors.
    bool leftReading, middleReading, rightReading, bottomReading;
    leftReading = leftSensor->read();
    middleReading = middleSensor->read();
    rightReading = rightSensor->read();
    bottomReading = bottomSensor->read();

    if(leftReading || middleReading || rightReading) {
      // At least one sensor is seeing black, update direction.
      // This code sets this->turn to either -2, -1, 0, 1 or 2 depending on the sensors. See if you can figure it out.
      float turnF = 0.;
      if(leftReading) turnF += -1.;
      if(rightReading) turnF += 1.;
      if(middleReading) turnF = turnF/2.;

      this->turn = turnF*2;
    }

    // This code updates the motor speeds and directions depending on the decided degree of turning.
    command.leftMotorDirection = true;
    command.rightMotorDirection = true;
    unsigned char forwardSpeed = 80;
    unsigned char turnSpeed = 70;
    unsigned char tightSpeed = 75;
    switch(this->turn) {
      case -2: // we need to turn sharply left
        command.rightMotorSpeed = tightSpeed;
        command.leftMotorSpeed = tightSpeed;
        command.leftMotorDirection = false;
        break;
      case -1: // we need to turn left
        command.rightMotorSpeed = turnSpeed;
        command.leftMotorSpeed = 0;
        break;
      case -0: // we need to go straight
        command.rightMotorSpeed = forwardSpeed;
        command.leftMotorSpeed = forwardSpeed;
        break;
      case 1: // we need to turn right
        command.rightMotorSpeed = 0;
        command.leftMotorSpeed = turnSpeed;
        break;
      case 2: // we need to turn sharply right
        command.rightMotorDirection = false;
        command.rightMotorSpeed = tightSpeed;
        command.leftMotorSpeed = tightSpeed;
        break;
      default:
        command.rightMotorSpeed = 0;
        command.leftMotorSpeed = 0;
    }

    return command;
  }

private:
  char turn;
  float controlGain;
  LightSensor *rightSensor, *leftSensor, *middleSensor, *bottomSensor;
};