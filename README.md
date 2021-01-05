<h2>RemoteDisplay</h2>
<p>This library has a lofty purpose - to allow microcontroller boards to serve
as clients or servers for remote displays and input (buttons). Why? I
frequently find myself experimenting with some code and then having to cobble
up a display and some buttons to test a project idea. This leads to time wasted
wiring up 0.1" perfboards with shaky connections and frustration. The idea of
the library is to allow easy output and input for your microcontrollers with
the simplest of interfaces that can be remote (over wired or wireless links)
or the same code and work the same way with directly connected hardware. This
allows you to easily prototype ideas without owning the final hardware to then
turn it into a finished product/project on the actual hardware and have it work
the same way.</p>
<br>
<h2>Features</h2>
- Supports all common OLED and LCD displays<br>
- Provides a rich set of graphics primitives<br>
- 5 built-in fonts (6x8, 8x8, 12x16, 16x16, 16x32)<br>
- Supports Adafruit_GFX format custom fonts<br>
- Efficient system for sending graphics assets over the air/wire<br>
- Works equally on the 3 main BLE APIs (Arduino, ESP32, Adafruit)<br>
- Includes example display servers for iOS/MacOS and ESP32<br>
<br>
<h2>Goals</h2>
Initially, this project was to explore using Bluetooth low energy (BLE)
as a transport for a responsive and interactive system on low cost MCUs. I've
learned a lot along the journey and had to create some workarounds to bugs and
limitations that I found. After all the time I've spent on BLE, I've come to
the conclusion that it is fast enough and reliable enough to use for a lot
of purposes that the creators might not have imagined. The simplicity of
having short range wireless communication without the need for authentication
or user intervention makes it ideal for simple point-to-point
activities. Since creating the basic infrastructure of this library, I
welcome community involvement to grow this into something truly useful for
the whole embedded/Arduino community.<br>


