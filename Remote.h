#pragma once
#include <SoftwareSerial.h>  

// This class represents the remote control and updates its internal state to match the controllers by receiving updates via bluetooth.
class Remote {
public:
  Remote(int timeout, unsigned char tx, unsigned char rx) {
    this->timeout = timeout;
    this->state.connected = false;
    lastSeen = 0;

    // ifdef stuff is some preprocessor magic. The code can be compiled so the bluetooth module uses hardware serial or software serial, this handles that.
    #ifdef BT_HW_SERIAL
      Serial.begin(115200);
    #else
      bluetooth = new SoftwareSerial(tx, rx);

      bluetooth->begin(115200);
    #endif
  }

  enum Direction {
    LEFT,
    FORWARDS,
    RIGHT,
    BACKWARDS
  };

  // Defining a data structure containing the state of the controller and wether it's connected or not.
  struct State {
    unsigned char speed;
    Direction dir;
    bool fire;
    bool manual;
    bool drive;
    bool connected;
  };

  // This function receives new updates from the controller if they are available, updates the internal state and checks if the last update was longer ago than the timeout, in which case it's assumed connection has been lost.
  void tick() {
    #ifndef BT_HW_SERIAL
      if(bluetooth->available()) {
    #else
      if(Serial.available()) {
    #endif
      unsigned char controlByte = 
      #ifdef BT_HW_SERIAL // Some preprocessor magic deciding wether to receive byte from hardware or software serial.
        Serial.read();
      #else
        bluetooth->read();
      #endif
      parseControlByte(controlByte); // Pass the byte of to be parsed and the internal state updated.
      this->state.connected = true; // We just received a byte, so we are connected.
      lastSeen = millis();
    }

    // This checks wether we have timed out on the connection and updates the internal state if we have.
    if(millis() - lastSeen > timeout) {
      #ifndef BT_HW_SERIAL
        if(this->state.connected) {Serial.print("Warning! Remote has not been seen for over "); Serial.print(timeout); Serial.println("ms.");}
      #endif
      this->state.connected = false;
    }
  }

  // Just give a copy of the internal state.
  State getState() {
    return this->state;
  }

private:
  // This function might be a little hard to understand. The state from the controller is received in a single byte, with multiple booleans and numbers packed together,
  // this function unpacks that and updates the internal state. If you want to understand it, search for "C++ bit shift operator", "Bit mask" and "Bitwise AND".
  void parseControlByte(unsigned char controlByte) {
    unsigned char mask = 0b00000001;

    this->state.drive = ((controlByte & mask) == 1);

    controlByte = controlByte >> 1;
    this->state.fire = ((controlByte & mask) == 1);

    controlByte = controlByte >> 1;
    this->state.manual = ((controlByte & mask) == 1);

    mask = 0b00000111;
    controlByte = controlByte >> 1;
    this->state.speed = (controlByte & mask);

    mask = 0b00000011;
    controlByte = controlByte >> 3;
    this->state.dir = (Direction)(controlByte & mask);
  }

  SoftwareSerial* bluetooth;
  State state;

  unsigned long lastSeen;
  int timeout;
};
