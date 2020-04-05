# Oscilloscope-Clock
Arduino-based analog clock drawn on an oscilloscope in X-Y mode
   
[![Demo](http://img.youtube.com/vi/hR9PfUYebpU/0.jpg)](http://www.youtube.com/watch?v=hR9PfUYebpU "Demo")

Generates analog signals via a low-pass filter of PWM output.<br>
Clock can be advanced by a push button. <br>
DELAY may need to be tuned for your scope (I used a Tektronix TBS 1202B-EDU).<br>

<img src="https://github.com/daveyburke/Oscilloscope-Clock/blob/master/Schematic.jpg" alt="Schematic" width="400"/>

TODO: Use an R2R ladder for DAC and a Real-Time Clock to make this better. 

(Inspired by https://neil.fraser.name/news/2018/clock.html and https://blog.arduino.cc/2018/12/27/draw-on-an-oscilloscope-with-arduino/)
