#pragma once

// A pure virtual class that represents a generic control command source. (Implementations are in ManualControl.h and Autonomy.h).
class Controller {
public:
  struct ControlCommand {
    unsigned char leftMotorSpeed;
    unsigned char rightMotorSpeed;
    bool leftMotorDirection;
    bool rightMotorDirection;
  };

  virtual ControlCommand update() = 0;
};