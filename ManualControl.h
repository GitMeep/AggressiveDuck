#pragma once

#include "Controller.h"
#include "Remote.h"

// This is a class that takes the remote controller state and gives a command to control the robot.
class ManualControl : public Controller {
public:
  ManualControl(Remote *remote, unsigned int firePin) {
    this->remote = remote;
    this->firePin = firePin;
  }

  ControlCommand update() {
    ControlCommand command;
    Remote::State control = remote->getState();
    
    // If the drive button is held, look at the direction and choose the appropriate motor directions and speeds.
    if(control.drive) {
      switch(control.dir) {
        case Remote::FORWARDS:
          command.rightMotorDirection = true;
          command.leftMotorDirection = true;
          command.leftMotorSpeed = map(control.speed, 0, 7, 0, 170/2);
          command.rightMotorSpeed = map(control.speed, 0, 7, 0, 255/2);
          break;
        case Remote::BACKWARDS:
          command.rightMotorDirection = false;
          command.leftMotorDirection = false;
          command.leftMotorSpeed = 100;
          command.rightMotorSpeed = 100;
          break;
        case Remote::LEFT:
          command.rightMotorDirection = true;
          command.leftMotorDirection = false;
          command.leftMotorSpeed = map(control.speed, 0, 7, 0, 80);
          command.rightMotorSpeed = map(control.speed, 0, 7, 0, 170);
          break;
        case Remote::RIGHT:
          command.rightMotorDirection = false;
          command.leftMotorDirection = true;
          command.leftMotorSpeed = map(control.speed, 0, 7, 0, 170);
          command.rightMotorSpeed = map(control.speed, 0, 7, 0, 80);
          break;
        default:
          command.leftMotorSpeed = 0;
          command.rightMotorSpeed = 0;
      }
    } else { // If the drive button is not held, just stop the motors.
      command.leftMotorSpeed = 0;
      command.rightMotorSpeed = 0;
    }

    // If the fire button is held, DIVERT POWER TO CANNONS (there's only one).
    if(control.fire) {
      analogWrite(firePin, 80);
    } else {
      analogWrite(firePin, 0);
    }

    return command;
  }

private:
  Remote* remote;
  unsigned int firePin;
};