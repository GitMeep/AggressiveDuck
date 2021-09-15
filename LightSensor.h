#pragma once

// This class represents a light sensor. It has the pin that it's connected to, it's individual black/white threshold and has a filter for smoothing the value.
class LightSensor {
public:
  LightSensor(unsigned char pin, float threshold, float p) {
    this->pin = pin;
    this->threshold = threshold;
    this->p = p;

    pinMode(pin, INPUT);
    filteredReading = analogRead(pin);
  }

  // This reads the sensor and updates the internal value to be closer to it, with p determining how much closer. It is essentially a proportional controller.
  void update() {
    filteredReading += (((float)analogRead(pin)) - filteredReading) * p;
  }

  // Gets the raw internal filtered reading.
  float getFilteredReading() {
    return this->filteredReading;
  }

  // Is the internal reading below the threshold? (Is the sensor seeing black?).
  bool read() {
    return (filteredReading < threshold);
  }

private:
  float p;
  float filteredReading;
  unsigned char pin;
  float threshold;
};