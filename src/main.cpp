#include <avr/pgmspace.h>
#include <Arduino.h>
#include <Wire.h>
#include "RtcDS3231.h"

#define PUMP_PIN 13
#define DELAY_TIME 10000
#define BAUD_RATE 9600
#define WATERING_TIME 30000 // 30 sec
#define SQW_PIN 2

#define countof(a) (sizeof(a) / sizeof(a[0]))

RtcDS3231<TwoWire> Rtc(Wire);

volatile boolean alarmIsrWasCalled = false;

void alarmIsr()
{
    alarmIsrWasCalled = true;
    Serial.println("alarmIsr() called.");
}

void watering() {
    digitalWrite(PUMP_PIN, HIGH);
    delay(WATERING_TIME);
    digitalWrite(PUMP_PIN, LOW);
}

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring,
        countof(datestring),
        PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
        dt.Month(),
        dt.Day(),
        dt.Year(),
        dt.Hour(),
        dt.Minute(),
        dt.Second());

    Serial.println(datestring);
}

void rtcInit() {
    pinMode(SQW_PIN, INPUT_PULLUP);

    Rtc.Begin();

    auto compiledTime = RtcDateTime(__DATE__, __TIME__);
    Serial.print("Compiled time: ");
    printDateTime(compiledTime);

    if (!Rtc.IsDateTimeValid())
    {
        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiledTime);
    }

    if (!Rtc.GetIsRunning())
    {
      Serial.println("RTC was not actively running, starting now");
      Rtc.SetIsRunning(true);
    }

    auto now = Rtc.GetDateTime();
    if (now < compiledTime)
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiledTime);
    }

    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeAlarmOne);

    // everyday at 11:00 o'clock
    DS3231AlarmOne alarmOne(1, 11, 0, 0, DS3231AlarmOneControl_HoursMinutesSecondsMatch);
    Rtc.SetAlarmOne(alarmOne);

    // throw away any old alarm state before we ran
    Rtc.LatchAlarmsTriggeredFlags();

    attachInterrupt(INT0, alarmIsr, FALLING);
}

void rtcSetDateTime() {
    // Format: "Dec 26 2009|12:34:56"
    Serial.println("Set current date and time.");
    auto str = Serial.readStringUntil(PSTR("\r\n"));
    while (Serial.available() > 0) Serial.read();
    str.trim();
    auto index = str.indexOf('|');
    auto dateStr = str.substring(0, index);
    auto timeStr = str.substring(index + 1);
    auto dt = RtcDateTime(dateStr.c_str(), timeStr.c_str());
    Rtc.SetDateTime(dt);
}

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("Program start...");

  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);

  rtcInit();
}

void loop() {
  if (Serial.available() > 0) {
    rtcSetDateTime();
  }

  if (alarmIsrWasCalled) {
      alarmIsrWasCalled = false;
      printDateTime(Rtc.GetDateTime());

      Serial.println("Watering time!");
      watering();
  }

  delay(DELAY_TIME);
}
