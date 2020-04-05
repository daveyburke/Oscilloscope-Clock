/*******************************************************************************
/* Draw clock on an oscilloscope
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
#define DELAY 250

#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)

#define RADIUS 120.0
#define XY_OFFSET RADIUS

long gBootTime = 0;
long gTimeOffset = 0;
int gLastButtonState = LOW;

// Draw a point. Origin at (0, 0), positive values
void drawPoint(int x, int y) {
  analogWrite(X_PIN, x + XY_OFFSET); 
  analogWrite(Y_PIN, y + XY_OFFSET);
  delayMicroseconds(DELAY);
}

// Draw line from radius r1 to radius r2 (angles measured in turns)
void drawRadialLine(float angle, int r1, int r2) {
  int dir = r1 > r2 ? -4 : 2;
  for (int r = r1; r1 > r2 ? r > r2 : r < r2; r += dir) {
    drawPoint(sin(2 * PI * angle) * r, cos(2 * PI * angle) * r);
  }
}
// Draw arc from angle a1 to a2 (angles measured in turns) with radius r
void drawArc(float a1, float a2, float r) {
  if (a2 < a1) {
    a2 += 1;
  }
  for (float angle = a1; angle < a2; angle += 0.005) {
    int x = sin(2 * PI * angle) * r;
    int y = cos(2 * PI * angle) * r;
    drawPoint(x, y);    
  }
}

void setup() {
  pinMode(X_PIN, OUTPUT);
  pinMode(Y_PIN, OUTPUT);
  pinMode(CLOCK_ADV_PIN, INPUT);

  // Adjust PWM for pins 5, 6 to 62500 hz. This will affect millis() by a factor of 64
  // https://playground.arduino.cc/Main/TimerPWMCheatsheet/
  TCCR0B = (TCCR0B & 0b11111000) | 0x01;
  
  gBootTime = millis();
}

void loop() {
  long elapsedSec = gTimeOffset + (millis() - gBootTime) / 64000; // adjusted for timer change
  int hrs = numberOfHours(elapsedSec);
  int mins = numberOfMinutes(elapsedSec);
  int secs = numberOfSeconds(elapsedSec);
  
  // Second hand out, draw clock circle, back to center
  drawRadialLine(secs / 60.0, 0, RADIUS);
  drawArc(secs/60.0, (secs / 60.0) + 1, RADIUS);
  drawRadialLine(secs / 60.0, RADIUS, 0);
  
  // Minute hand out/back
  drawRadialLine(mins / 60.0, 0, RADIUS - 10);
  drawRadialLine(mins / 60.0, RADIUS - 10, 0);
  
  // Hours out/back
  drawRadialLine(hrs / 12.0, 0, RADIUS - 30);
  drawRadialLine(hrs / 12.0, RADIUS - 30, 0);
  
  if (digitalRead(CLOCK_ADV_PIN) == HIGH) {
    gTimeOffset += (gLastButtonState == LOW) ? SECS_PER_MIN : SECS_PER_MIN * 15;
    gLastButtonState = HIGH;
  } else {
    gLastButtonState = LOW;
  }
}
