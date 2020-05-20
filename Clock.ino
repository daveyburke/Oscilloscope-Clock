/*******************************************************************************
/* Draw an analog clock on an oscilloscope
/* (Dave Burke, 2020)
/* 
/* Use an RC LPF on each output (4k / 0.1 uF). XY mode on scope
/* Connect pin 2 with pull down resistor to ground and pulse with 5V to set time
/*
/* Define RTC for optional DS32321 real-time clock for accurate time keeping.
/* Wire to SDA, SCL lines. Without RTC, clock is accurate to ~1 min / day.
/*
/* Inspired by:
/*   https://neil.fraser.name/news/2018/clock.html 
/*   https://blog.arduino.cc/2018/12/27/draw-on-an-oscilloscope-with-arduino/
/*
/* DELAY may need to be tuned for your scope
/*
/* TODO: Use an R2R ladder for DAC
/* *****************************************************************************/

#include <Wire.h>
#include <DS3231.h>

#define RTC  // define for optional DS3231 real-time clock

#define X_PIN 5  
#define Y_PIN 6 
#define CLOCK_ADV_PIN 2
#define DELAY 30

#define RADIUS 125.0
#define XY_OFFSET RADIUS

#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)

int gLastButtonState = LOW;
unsigned long gBootTimeSec = 0;
unsigned long gTimeOffset = 0;
DS3231 gRtc;

// 64-bit millis() to work around rollover (happens ~daily due to faster timer)
uint64_t millis64() {
    static uint32_t low32, high32; 
    uint32_t newLow32 = millis();
    if (newLow32 < low32) high32++; 
    low32 = newLow32;
    return ((uint64_t) high32 << 32 | low32) / 64UL;
}

void setRtcTime(int year, int month, int date, int hour, int minute, int second) {
    gRtc.setYear(year);
    gRtc.setMonth(month);
    gRtc.setDate(date);
    gRtc.setHour(hour);
    gRtc.setMinute(minute);
    gRtc.setSecond(second);
}

void advanceRtcTime(int mins) {
    bool h12, pm;
    int newMins = gRtc.getMinute() + mins;
    int newHours = gRtc.getHour(h12, pm) + (newMins >= 60 ? 1 : 0);
    gRtc.setSecond(0);
    gRtc.setMinute(newMins % 60);
    gRtc.setHour(newHours % 12);
}

// Draw a point. Origin at (0, 0), positive values
void drawPoint(int x, int y) {
    analogWrite(X_PIN, x + XY_OFFSET); 
    analogWrite(Y_PIN, y + XY_OFFSET);
    delayMicroseconds(DELAY);
}

// Draw line from radius r1 to radius r2 at angle (angles measured in turns)
void drawRadialLine(float angle, int r1, int r2) {
    int dir = r1 > r2 ? -4 : 4;
    for (int r = r1; r1 > r2 ? r > r2 : r < r2; r += dir) {
        drawPoint(sin(2 * PI * angle) * r, cos(2 * PI * angle) * r);
    }
}

// Draw arc from angle a1 to a2 with radius r (angles measured in turns)
void drawArc(float a1, float a2, float r) {
    float dir = a2 > a1 ? .005 : -.005;
    for (float a = a2 > a1 ? a1 : a2; a2 > a1 ? a < a2 : a > a1; a += dir) {
        drawPoint(sin(2 * PI * a) * r, cos(2 * PI * a) * r);
    }
}

void setup() {
    pinMode(X_PIN, OUTPUT);
    pinMode(Y_PIN, OUTPUT);
    pinMode(CLOCK_ADV_PIN, INPUT);

    // Adjust PWM for pins 5, 6 to 62500 Hz. This will affect millis() by a factor of 64
    // https://playground.arduino.cc/Main/TimerPWMCheatsheet/
    TCCR0B = (TCCR0B & 0b11111000) | 0x01;

#ifdef RTC
    Wire.begin();
    Serial.begin(9600);
    //setRtcTime(2020, 4, 25, 12, 0, 0);
#else
    gBootTimeSec = millis64() / 1000UL;  // adjusted for faster timer
#endif
}

void loop() {  
#ifdef RTC
    bool h12, pm;  
    int hrs = gRtc.getHour(h12, pm);
    int mins = gRtc.getMinute();
    int secs = gRtc.getSecond();
#else
    unsigned long elapsedSec = millis64() / 1000UL - gBootTimeSec + gTimeOffset;
    int hrs = numberOfHours(elapsedSec);
    int mins = numberOfMinutes(elapsedSec);
    int secs = numberOfSeconds(elapsedSec);
#endif

    if (digitalRead(CLOCK_ADV_PIN) == HIGH) {
#ifdef RTC
        advanceRtcTime((gLastButtonState == LOW) ? 1 : 10);
#else
        gTimeOffset += (gLastButtonState == LOW) ? SECS_PER_MIN : SECS_PER_MIN * 10;
#endif
        gLastButtonState = HIGH;
    } else {
        gLastButtonState = LOW;
    }
  
    // Drawing logic. Trick is to draw everything
    // without "lifting the pen" and end up in
    // the same spot you started from.
  
    // Second hand
    float secsAngle = secs / 60.0;
    drawRadialLine(secsAngle + 0.5, 0, 30);  // reverse part of hand
    drawRadialLine(secsAngle, 0, RADIUS);
    
    // Tick marks
    for (int i = secs; i < secs + 60; i++) {
        float a = i / 60.0;
        drawRadialLine(a, RADIUS, RADIUS - (i % 5 == 0 ? 30 : 10));  // long ticks for hours
        drawRadialLine(a, RADIUS - (i % 5 == 0 ? 30 : 10), RADIUS);
        drawArc(a, a + 1 / 60.0, RADIUS);
    }
    drawRadialLine(secsAngle, RADIUS, 0);
      
    // Minute hand out/back
    float minsAngle = mins / 60.0;
    drawRadialLine(minsAngle + 0.5, 0, 30);
    drawRadialLine(minsAngle, 0, RADIUS - 10);
    drawRadialLine(minsAngle, RADIUS - 10, 0);
  
    // Hours hand out/back
    float hrsAngle = hrs / 12.0 + minsAngle / 12.0;
    drawRadialLine(hrsAngle + 0.5, 0, 30);
    drawRadialLine(hrsAngle, 0, RADIUS - 45);
    drawRadialLine(hrsAngle, RADIUS - 45, 0);
}
