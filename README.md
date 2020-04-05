# Oscilloscope-Clock
Arduino-based analog clock drawn on an oscilloscope in X-Y mode

Inspired by:
   https://neil.fraser.name/news/2018/clock.html
   https://blog.arduino.cc/2018/12/27/draw-on-an-oscilloscope-with-arduino/

Generates analog signals via a low-pass filter of PWM output.
Clock can be advanced by a push button. 

![Schematic](https://octodex.github.com/images/yaktocat.png)

DELAY may need to be tuned for your scope. Mine is a Tektronix TBS 1202B-EDU

TODO: Use an R2R ladder for DAC and a Real-Time Clock
