# Oscilloscope-Clock
Arduino-based analog clock drawn on an oscilloscope in X-Y mode

Inspired by:
   https://neil.fraser.name/news/2018/clock.html and<br>
   https://blog.arduino.cc/2018/12/27/draw-on-an-oscilloscope-with-arduino/
   
[![Demo](http://img.youtube.com/vi/hR9PfUYebpU/0.jpg)](http://www.youtube.com/watch?v=hR9PfUYebpU "Demo")

Generates analog signals via a low-pass filter of PWM output.
Clock can be advanced by a push button. 
DELAY may need to be tuned for your scope (I used a Tektronix TBS 1202B-EDU)

<img src="https://github.com/daveyburke/Oscilloscope-Clock/blob/master/Schematic.jpg" alt="Schematic" width="400"/>

TODO: Use an R2R ladder for DAC and a Real-Time Clock to make this better. 
