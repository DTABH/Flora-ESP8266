# Conclusion  Hints to major problems building the watch and software:
*Conclusion*
<ul> Thanks to  mcer12 and his Flora-ESP8266, i build now 2 of these clocks. <br>
  Due to failure of internet and reboot of the clock, the clock don't works 
  and i decided to implement an RTC-Module <br>
  It was partially a good experience even i had some problems with hard and software. <br>
  Perhaps i will build my first own pcb with some changes:  <br>
  <li> Using an external potentiometer or a switch for different voltages so i could dimm very low.     
  </li>
  <li> RTC and buttons if possible on the backside of the pcb
  </li>  
  <li> An easy soldering of the USB port 
  </li>  
</ul>

*Hardware*
<ul>
   <li> Having bad or wrong MT3608, took me .. 6.. hours and 6 bad MT3608 i killed  <br>
     and 3 MT3608 which can't get higher then 18 Volt even it was specified on the package! <br>
     (The MT3608L (L for low?) is specified only up to 20.)
  </li>
  <li> Wrong placement of Resistors R3 an R4 leads to impossible programm upload took me 2 -3 days to find out.  <br>
    Desoldered CH340 and ESP2M and replaced with new... no idea why it doesn't work <br>
    replaced transistors, capacitors, tried to find out something with the ozsi  <br>
    Many wrong approaches .... then suddenly i saw the Bullshit i made ....
  </li>  
  <li> One segment always on with brightness high due to bad 74HC595D. <br>
    Changing the 74HC595D and it works.
  </li>
  <li> Soldering the USB-Port is absolutly bullshit. Absolutly.  Don't do it.
  </li>
</ul>

*Software*
<ul>
   <li> Having done everything what to do, the program crashes after successful build and upload.
     Took me several hours and many Serial.println(.. to get closer to the problem. <br>
     It was: ICACHE_RAM_ATTR was no longer supported and simply ignored. Bullshit <br>
     So i replaced it as suggested with IRAM_ATTR. also in SPI.h !!! and the program works.     
  </li>
  <li>  [[maybe_unused]] in the library of NeoPixelBus_by_Makuna leads to compiler errors.  <br>
    I don't know how to deactivate this compiler behaviour in Arduino, so i deleted  [[maybe_unused]] in the hole library and the code could be compiled.    
  </li>  
</ul>
# Changes:

**FW_VERSION "6.1.0 dtabh"**

<ul>
   <li> Final version
  </li>
  <li> Bugfix : Without RTC program hangs and wdt will continously reset the esp.    
  </li>  
</ul>

**FW_VERSION "6.0.3 dtabh"**

<ul>
  <li> Brightness low is always 1 point lower to get 1 for lowest brightness
  </li>
  <li>Nightmode changed in start hour of night mode and ending hour new implemented     
    Actual start hours are 19,20,21,22 end hours 5,6,7,8,9 .
      Only works correct in 24 h mode
  </li>  
  <li>Bugfix: Indifferent crashed of Webpage with restart of the clock
  </li>
</ul>

**FW_VERSION "6.0.2 dtabh"**
<ul>
  <li>Red Colon color during menu input.
  </li>
   <li>Bugfix: Toggle with button3 to switch on/off Nightmode with automatic reset of toggle to normal at 6'clock.
  </li>
</ul>

**FW_VERSION "6.0.1 dtabh"**

<ul>
  <li>Lowest Brightness is now 2 and steps also 2. ( Clock in the night should be very very dark)
  </li>
  <li>Configuration of medium brightness possible with configuration on webpage.
  </li>
   <li>Showing day and month every 30 or 60 seconds with configuration on webpage.
  </li>
   <li>Extended menu for Nightmode  with begin hour 19,20,21,22  with configuration on webpage.
  </li>
   <li>Toggle with button3 to switch on/off Nightmode with automatic reset of toggle to normal at 6'clock.
  </li>
   <li>Possible bug : opening Webpage after reprogramming leads to restart of the clock, until FireFox browser cache is flashed
After flashing the browser cache everythings ok.
  </li>
</ul>

**FW_VERSION "6.0.0 dtabh"**

<ul>
  <li>Implemented  a cheap china RTC Modul (see Pictures) using 2 free GPIO of the ESP. 
    GPIO 4 and 5 are by default already SDA and SCL for the clock. <br>
    If a NTP connection exists, the time from NTP will be used (RTC is synchonized). <br>
    If NTP fails RTC ist used. This mixed mode will only work if a WLAN connection exists. <br>
    If no WLAN exists a RTC-Only mode must/can be activated pressing Button 1+2 together. <br>
  </li>
  <li>Implemented 3 Buttons (see Pictures) using 3 other GPIO 12 16 and AD1. 
  </li>
  <li>
    For the buttons a menu was implemented to set the RTC-Clock without WLAN/NTP connection. <br>
    The minute, hour,year,month and day must be set manually pressing button 1 first <br>
    and then 2 for "+" and 3 for "-". for changing the values <br>
    Pressing button1 again steps further until day at least. (if NTP has once worked before RTC is already set). <br> 
  </li>
</ul>
<br>
<br>
<br>
<br>

**Images:**

![alt text](https://raw.githubusercontent.com/dtabh/Flora-ESP8266/main/Images/DS3231_RTC-Clock.jpg)  
![alt text](https://raw.githubusercontent.com/dtabh/Flora-ESP8266/main/Images/IMG_1231.jpg)  
![alt text](https://raw.githubusercontent.com/dtabh/Flora-ESP8266/main/Images/IMG_1232.jpg)  
![alt text](https://raw.githubusercontent.com/dtabh/Flora-ESP8266/main/Images/IMG_1233.jpg)  
![alt text](https://raw.githubusercontent.com/dtabh/Flora-ESP8266/main/Images/IMG_1234.jpg)  
![alt text](https://raw.githubusercontent.com/dtabh/Flora-ESP8266/main/Images/IMG_1235.jpg)  
![alt text](https://raw.githubusercontent.com/dtabh/Flora-ESP8266/main/Images/IMG_1236.jpg)  

<br>
<br>
<br>
<br>
<br>
<br>

# --------------- original Readme  ---------------
# Flora-ESP8266
Flora is an open source ESP8266 VFD clock, you can make a 4-digit IV-22 variant or 6-digit IV-6 variant. Designed to be low profile and as small as possible, using widely available components. 

**STATUS:**
- **IV-22 variant:** Tested and working, schematic, gerber BOM and 3D model of cover are included. Assembly guide added to wiki.
- **IV-6 variant:** Tested and working. Schematic, gerber and BOM and 3D model of cover are included.
- **IV-12 variant (thin):** Tested and working. Schematic, gerber and BOM and 3D model of cover are included.
- **IV-6 V2 variant (thin):** Tested and working. Schematic, gerber and BOM and 3D model of cover are included.
- **IV-3 variant (thin):** Tested and working. Schematic, gerber and BOM and 3D model of cover are included.

**Wiki**  
https://github.com/mcer12/Flora-ESP8266/wiki/

**IV-22 enclosure model:**  
https://www.thingiverse.com/thing:4744087

**IV-6 enclosure model:**  
https://www.thingiverse.com/thing:5213794

**IV-6 V2 and IV-3 enclosure model:**  
https://www.printables.com/model/477693-flora-iv-6-v2-and-iv-3-clock-case

**IV-12 enclosure model:**  
https://www.thingiverse.com/thing:4890505

**Some things to note:**
1) This design is made with small footprint and ultra low-profile in mind, using only 3mm high components.
2) Using modern and widely available components.
3) Can be completely sourced from LCSC (except for the VFDs of course) and partially assembled using JLCPCB assembly service.
4) Colons made out of WS2812 addressable leds
5) CH340 with NodeMCU style auto-reset built-in for easy programming / debugging
6) Each segment driven directly, not multiplexed
7) Brightness balancing - easy to match tubes with different brightness
8) NTP based, no battery / RTC. Connect to wifi and you're done.
9) Simple and easy to set up, mobile-friendly web interface
10) diyHue and remote control support
11) 100hz refresh rate using HW ISR timer, zero flicker and not affected by wifi activity.
12) 3 levels of brightness, each with 8 more levels for dimming / crossfade. 48 pwm steps in total for each segment!
13) No buttons design. Simple configuration portal is used for settings.
14) Daylight saving (summer time) support built in and super simple to set up.
15) MicroUSB connector (USB type C on IV-12 version), Below 500mA power consumption on 5V on full brightness (800mA on IV-12 version).


**Images:**

![alt text](https://raw.githubusercontent.com/mcer12/Flora-ESP8266/main/Images/IV12_1.jpg)  

![alt text](https://raw.githubusercontent.com/mcer12/Flora-ESP8266/main/Images/IV22_1.jpg)  

![alt text](https://raw.githubusercontent.com/mcer12/Flora-ESP8266/main/Images/IV6_1.jpg)  

![alt text](https://raw.githubusercontent.com/mcer12/Flora-ESP8266/main/Images/screenshot.png)  

**License:**  
GPL-3.0 License  
LGPL-2.1 License (modified SPI library)  
Mozilla Public License 2.0 (iro.js, https://github.com/jaames/iro.js)
