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
        this->countdownStart = millis();
      } else if(millis() - countdownStart > 1000) {
        this->searching = true;
        this->searchState = 0;
        this->countingDown = false;
        command.leftMotorSpeed = 0;
        command.rightMotorSpeed = 0;
        return command;
      }
    }

    // This code updates the motor speeds and directions depending on the decided degree of turning.
    command.leftMotorDirection = true;
    command.rightMotorDirection = true;
    switch(this->turn) {
      case -2: // we need to turn sharply left
        command.rightMotorSpeed = tightSpeed;
        command.leftMotorSpeed = tightSpeed;
        command.leftMotorDirection = false;
        break;
      case -1: // we need to turn left
        command.leftMotorDirection = false;
        command.rightMotorSpeed = turnSpeed;
        command.leftMotorSpeed = turnSpeed/3;
        break;
      case -0: // we need to go straight
        command.rightMotorSpeed = forwardSpeed;
        command.leftMotorSpeed = forwardSpeed;
        break;
      case 1: // we need to turn right
        command.rightMotorDirection = false;
        command.rightMotorSpeed = turnSpeed/3;
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

  // This function gets called each tick when the line has been determined to be lost
  ControlCommand search(bool leftReading, bool middleReading, bool rightReading, bool bottomReading) {
    ControlCommand command;

    if(!(middleReading || leftReading || rightReading)) {
      switch(this->searchState) {
        case 0:
          this->searchTry = 1;
          this->searchTimeMultiplier = 1;
          this->searchState = 4;
          this->searchTimer = millis();
          break;

        case 4:
          if(millis() - searchTimer > 400) {
            this->searchState = 1;
            this->searchTimer = millis();
            break;
          }
          command.leftMotorDirection = false;
          command.rightMotorDirection = true;
          command.leftMotorSpeed = 80;
          command.rightMotorSpeed = 80;
          break;

        case 1:
          if(millis() - searchTimer > 1000 * searchTimeMultiplier) {
            this->searchState = 2;
            this->searchTimer = millis();
            break;
          }
          command.leftMotorDirection = true;
          command.rightMotorDirection = true;
          command.leftMotorSpeed = 80;
          command.rightMotorSpeed = 80;
          break;

        case 2:
          if(millis() - searchTimer > 1000 * searchTimeMultiplier) {
            this->searchState = 3;
            this->searchTimer = millis();
            break;
          }
          command.leftMotorDirection = false;
          command.rightMotorDirection = false;
          command.leftMotorSpeed = 80;
          command.rightMotorSpeed = 80;
          break;
        
        case 3:
          if(millis() - searchTimer > 200) {
            if(this->searchTry >= 8) {
              this->searchTimeMultiplier++;
              this->searchTry = 1;
            } else {
              this->searchTry++;
            }
            this->searchState = 1;
            this->searchTimer = millis();
            break;
          }
          command.leftMotorDirection = false;
          command.rightMotorDirection = true;
          command.leftMotorSpeed = 80;
          command.rightMotorSpeed = 80;
          break;

        default:
          command.leftMotorDirection = false;
          command.rightMotorDirection = true;
          command.leftMotorSpeed = 255;
          command.rightMotorSpeed = 255;
      }
    } else {
      this->searching = false;
      command.leftMotorSpeed = 0;
      command.rightMotorSpeed = 0;
    }

    return command;
  }

  bool countingDown, searching;
  char turn;
  static const unsigned char forwardSpeed = 80, turnSpeed = 90, tightSpeed = 90;
  LightSensor *rightSensor, *leftSensor, *middleSensor, *bottomSensor;

  // Search-related member variables
  unsigned int searchTimeMultiplier;
  unsigned int searchTry;
  unsigned char searchState;
  unsigned long searchTimer;
  unsigned long countdownStart;
};