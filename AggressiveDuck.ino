// Uncomment if using the hardware serial to communicate with Bluetooth module. Unplug the modles TX and RX wires when programming the Arduino when using hardware serial.
//#define BT_HW_SERIAL

#include "Remote.h"
#include "LightSensor.h"
#include "Motor.h"
#include "Timer.h"
#include "ManualControl.h"
#include "Autonomy.h"
#include "Controller.h"

// Object representing the remote controller app. This deals with receiving updates from the app, mirroring the controller state and keeping track if it's still connected.
Remote* remote;

// Two different sources of commands to control the robot. One uses the remote object to tell the motors what to do, the other uses the sensor inputs.
Controller* manual;
Controller* autonomy;

// Objects representing each sensor. Each one keeps track of what pin the sensor uses and has some filtering that smooths out the readings.
LightSensor* sensor7;
LightSensor* sensor2;
LightSensor* sensor3;
LightSensor* sensor5;

// Very simple objects represesnting the motors.
Motor* leftMotor;
Motor* rightMotor;

// These objects are used to control how quickly updates are received from the remote and how quickly the robot updates it navigation (reads sensors, gets new commands from the controllers and updates motor speeds).
Timer* remoteTimer;
Timer* navTimer;

unsigned char firePin = 10;

// Function prototypes for timer handlers.
void updateRemote();
void updateNav();

void setup()
{
  #ifndef BT_HW_SERIAL
    Serial.begin(9600);
  #endif

  // Remote has a timeout of 500ms and uses TX: pin 7, RX: pin 6. (If not using hardware serial).
  remote = new Remote(500, 7, 6);

  // Each light sensor knows what pin it's connected to, what it's threshold between black and white is and how much the value is smoothed (lower is smoother).
  sensor7 = new LightSensor(A3, 900, 0.1);
  sensor2 = new LightSensor(A1, 900, 0.75);
  sensor3 = new LightSensor(A0, 900, 0.75);
  sensor5 = new LightSensor(A2, 900, 0.75);
  
  // Manual control gets the remote and the pin for firing the cannon. Autonomy gets pointers to all the sensors.
  manual = new ManualControl(remote);
  autonomy = new Autonomy(sensor2, sensor5, sensor3, sensor7);

  // Motors get their A and B pins as well as the enable pin for PWM.
  leftMotor = new Motor(5, 4, 11);
  rightMotor = new Motor(2, 3, 9);

  // The timers get their target period and handler functions.
  remoteTimer = new Timer(40, updateRemote);
  navTimer = new Timer(10, updateNav);
}

void loop() {
  // Each timer is ticked each loop. If it's time to execute it, the handler function gets called, otherwise nothing happens.
  remoteTimer->tick();
  navTimer->tick();
}

void updateRemote() {
  // Called from the remoteTimer. Checks for new state received from remote and updates internal state.
  remote->tick();
}

void updateNav() {
  // Get the latest state from the controller.
  Remote::State control = remote->getState();

  // Update all the sensor readings.
  sensor7->update();
  sensor2->update();
  sensor3->update();
  sensor5->update();

  // If we lost connection to the controller, stop the motors and return.
  if(!control.connected) {
    leftMotor->setSpeed(0);
    rightMotor->setSpeed(0);
    return;
  }

  // If the fire button is held, DIVERT POWER TO CANNONS (there's only one).
  if(control.fire) {
    analogWrite(firePin, 80);
  } else {
    analogWrite(firePin, 0);
  }

  // Prepare a command and get it from either the manual controller or autonomous controller.
  Controller::ControlCommand command;
  if(control.manual) {
    command = manual->update();
  } else {
    command = autonomy->update();
  }

  // Apply the command to the motors.
  leftMotor->setDirection(command.leftMotorDirection);
  rightMotor->setDirection(command.rightMotorDirection);
  leftMotor->setSpeed(command.leftMotorSpeed);
  rightMotor->setSpeed(command.rightMotorSpeed);
}
