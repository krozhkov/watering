
#include <Arduino.h>
#include "TimerOne.h"

#define PUMP_PIN 13
#define DELAY_TIME 2000
#define BAUD_RATE 9600
#define ONE_SECOND 1000000
#define PERIOD 86400 // 1 day
#define WATERING_TIME 60000 // 1 min

volatile unsigned long time;

void watering() {
    digitalWrite(PUMP_PIN, HIGH);
    delay(WATERING_TIME);
    digitalWrite(PUMP_PIN, LOW);
}

void setup() {
  Serial.begin(BAUD_RATE);
  Timer1.initialize(ONE_SECOND);
  Timer1.attachInterrupt([]{
    time++;
  });

  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);
  // first watering now!
  watering();
}

void loop() {
  Serial.println(time);
  if (time >= PERIOD) {
    time = 0;
    Serial.println("Watering time!");
    watering();
  }
  delay(DELAY_TIME);
}
