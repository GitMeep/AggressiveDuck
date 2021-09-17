#pragma once

#include "Controller.h"
#include "Remote.h"

// This is a class that takes the remote controller state and gives a command to control the robot.
class ManualControl : public Controller {
public:
  ManualControl(Remote *remote) {
    this->remote = remote;
  }

  ControlCommand update() {
    ControlCommand command;
    Remote::State control = remote->getState();
    
    // If the drive button is held, look at the direction and choose the appropriate motor directions and speeds.
    if(control.drive) {
      command.rightMotorDirection = true;
      command.leftMotorDirection = true;
      switch(control.dir) {
        case Remote::FORWARDS:
          command.leftMotorSpeed = map(control.speed, 0, 7, 0, 120);
          command.rightMotorSpeed = map(control.speed, 0, 7, 0, 120);
          break;
        case Remote::BACKWARDS:
          command.rightMotorDirection = false;
          command.leftMotorDirection = false;
          command.leftMotorSpeed = 100;
          command.rightMotorSpeed = 100;
          break;
        case Remote::LEFT:
          command.leftMotorSpeed = map(control.speed, 0, 7, 0, 80);
          command.rightMotorSpeed = map(control.speed, 0, 7, 0, 160);
          break;
        case Remote::RIGHT:
          command.leftMotorSpeed = map(control.speed, 0, 7, 0, 160);
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

    return command;
  }

private:
  Remote* remote;
};