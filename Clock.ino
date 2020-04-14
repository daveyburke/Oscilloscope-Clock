/*******************************************************************************
/* Draw an analog clock on an oscilloscope
/* (Dave Burke, 2020)
/* 
/* Use an RC LPF on each output (4k / 0.1 uF). XY mode on scope
/* Connect pin 2 with pull down resistor to ground and pulse with 5V to set time
/*
/* Inspired by:
/*   https://neil.fraser.name/news/2018/clock.html 
/*   https://blog.arduino.cc/2018/12/27/draw-on-an-oscilloscope-with-arduino/
/*
/* DELAY may need to be tuned for your scope
/*
/* TODO: Use an R2R ladder for DAC and a Real-Time Clock
/* *****************************************************************************/

#define X_PIN 5  
#define Y_PIN 6 
#define CLOCK_ADV_PIN 2
#define DELAY 80

#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)

#define RADIUS 125.0
#define XY_OFFSET RADIUS

unsigned long gBootTimeSec = 0;
unsigned long gTimeOffset = 0;
int gLastButtonState = LOW;

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

  // Adjust PWM for pins 5, 6 to 62500 hz. This will affect millis() by a factor of 64
  // https://playground.arduino.cc/Main/TimerPWMCheatsheet/
  TCCR0B = (TCCR0B & 0b11111000) | 0x01;
  
  gBootTimeSec = millis() / 64000UL; // adjusted for timer change
}

void loop() {
  unsigned long elapsedSec = millis() / 64000UL - gBootTimeSec + gTimeOffset;
  int hrs = numberOfHours(elapsedSec);
  int mins = numberOfMinutes(elapsedSec);
  int secs = numberOfSeconds(elapsedSec);
  
  if (digitalRead(CLOCK_ADV_PIN) == HIGH) {
    gTimeOffset += (gLastButtonState == LOW) ? SECS_PER_MIN : SECS_PER_MIN * 10;
    gLastButtonState = HIGH;
  } else {
    gLastButtonState = LOW;
  }
  
  // Drawing logic. Trick is to draw everything
  // without "lifting the pen" and end up in
  // the same spot your started from
  
  // Second hand out
  float secsAngle = secs / 60.0;
  drawRadialLine(secsAngle, 0, RADIUS);
  
  // Circumference from second hand to first tick
  float startTickAngle = ceil(secsAngle * 12) / 12.0;
  drawArc(secsAngle, startTickAngle, RADIUS);
  
  // Ticks and circumference arcs
  for (float i = startTickAngle; i < startTickAngle + 1; i += 1 / 12.0) {
    drawRadialLine(i, RADIUS, RADIUS - 10);
    drawRadialLine(i, RADIUS - 10, RADIUS);
    drawArc(i, i + 1 / 12.0, RADIUS);
  }
  
  // Move back to seconds angle and return to (0, 0)
  drawArc(startTickAngle, secsAngle, RADIUS); 
  drawRadialLine(secsAngle, RADIUS, 0);
  
  // Minute hand out/back
  float minsAngle = mins / 60.0;
  drawRadialLine(minsAngle, 0, RADIUS - 10);
  drawRadialLine(minsAngle, RADIUS - 10, 0);
  
  // Hours hand out/back
  float hrsAngle = hrs / 12.0 + minsAngle / 12.0;
  drawRadialLine(hrsAngle, 0, RADIUS - 30);
  drawRadialLine(hrsAngle, RADIUS - 30, 0);
}
