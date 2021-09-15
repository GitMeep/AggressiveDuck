#pragma once
// This defines a type TimerHandler that is a function pointer (the address of a piece of code) to call when the timer decides it is time.
typedef void (*TimerHandler)();

// This class represents a timer that can call a function on a certain interval.
class Timer {
public:
  Timer(unsigned long interval, TimerHandler handler) {
    this->interval = interval;
    this->handler = handler;
    lastRun = millis();
  }

  // Decides if it's time to run the timer based on when it was last run and the current time, and then updates what time it was last run at.
  void tick() {
    if(millis() - lastRun >= interval) {
      lastRun = millis();
      handler();
    }
  }

private:
  unsigned long lastRun;
  unsigned long interval;
  TimerHandler handler;
};