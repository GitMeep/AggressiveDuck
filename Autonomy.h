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

    this->countingDown = false;
    this->searching = false;
    this->turn = 0;
  }

  // Each time this controller is updated, it reads the sensors. If at least one sees black it decides which direction to go. If none see black, it continues in the last direction it had.
  ControlCommand update() {
    // Read all the sensors.
    bool leftReading, middleReading, rightReading, bottomReading;
    leftReading = leftSensor->read();
    middleReading = middleSensor->read();
    rightReading = rightSensor->read();
    bottomReading = bottomSensor->read();

    ControlCommand command;
    if(this->searching) {
      command = search(leftReading, middleReading, rightReading, bottomReading);
    } else {
      command = follow(leftReading, middleReading, rightReading, bottomReading);
    }

    return command;
  }

private:
  // This function gets called when the robot is on the line
  ControlCommand follow(bool leftReading, bool middleReading, bool rightReading, bool bottomReading) {
    ControlCommand command;
    if(leftReading || middleReading || rightReading) {
      // At least one sensor is seeing black, update direction.
      // This code sets this->turn to either -2, -1, 0, 1 or 2 depending on the sensors. See if you can figure it out.
      float turnF = 0.;
      if(leftReading) turnF += -1.;
      if(rightReading) turnF += 1.;
      if(middleReading) turnF = turnF/2.;

      this->turn = turnF*2;

      this->countingDown = false;
    } else if(!bottomReading) { // If the bottom sensor is also not seeing anything, go into search mode.
      if(!this->countingDown) {
        this->countingDown = true;
        this->countdown = millis();
      }
      if(millis() - countdown > 1000) {
        this->searching = true;
        return command;
      }
    }

    setTurn(command, this->turn, this->forwardSpeed, this->turnSpeed, this->tightSpeed);

    return command;
  }

  // This function gets called each tick when the line has been determined to be lost
  ControlCommand search(bool leftReading, bool middleReading, bool rightReading, bool bottomReading) {
    ControlCommand command;

    // If the bottom sensor finds the line again, turn on the spot until the middle sensor sees something
    if(bottomReading) {
      // If we are on the line again, check if the middle sensor is also on the line. If it is, stop searching, otherwise turn on the spot.
      if(middleReading) {
        this->searching = false;
        return command;
      }
      command.leftMotorDirection = true;
      command.rightMotorDirection = false;
      command.leftMotorSpeed = 85;
      command.rightMotorSpeed = 85;

      return command;
    }

    // If we don't see the line on the bottom sensor, continue turning in the same direction, but in reverse
    setTurn(command, this->turn, 70, 70, 70);
    command.leftMotorDirection = !command.leftMotorDirection;
    command.rightMotorDirection = !command.rightMotorDirection;

    return command;
  }

  void setTurn(ControlCommand& command, char turn, unsigned char forwardSpeed, unsigned char turnSpeed, unsigned char tightSpeed) {
    // This code updates the motor speeds and directions depending on the decided degree of turning.
    command.leftMotorDirection = true;
    command.rightMotorDirection = true;
    switch(turn) {
      case -2: // we need to turn sharply left
        command.rightMotorSpeed = tightSpeed;
        command.leftMotorSpeed = tightSpeed;
        command.leftMotorDirection = false;
        break;
      case -1: // we need to turn left
        command.leftMotorDirection = false;
        command.rightMotorSpeed = turnSpeed;
        command.leftMotorSpeed = turnSpeed/2;
        break;
      case -0: // we need to go straight
        command.rightMotorSpeed = forwardSpeed;
        command.leftMotorSpeed = forwardSpeed;
        break;
      case 1: // we need to turn right
        command.rightMotorDirection = false;
        command.rightMotorSpeed = turnSpeed/2;
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
  }

  bool countingDown, searching;
  unsigned int countdown;
  char turn;
  unsigned long searchStartTime;
  static const unsigned char forwardSpeed = 80, turnSpeed = 90, tightSpeed = 90;
  LightSensor *rightSensor, *leftSensor, *middleSensor, *bottomSensor;
};