## What do we have here ? ##
<center>![Content of current folder](http://i45.tinypic.com/2434u3p.png)</center>

## Going into some details ##
This folder contains the [eCAD](http://en.wikipedia.org/wiki/Electronic_design_automation) files -both schematic *.sch* and layout *.brd*- of the [USBTinyISP](http://www.ladyada.net/make/usbtinyisp/index.html) programmer **version 2.0**.  
From the developer's site, we find out the following about this circuit:

* It is an open-source hardware programmer for [*AVR*](http://en.wikipedia.org/wiki/Atmel_AVR) microcontrollers
* It is conceived as a *cheap and efficient* USB programmer for *AVR*s
* Unlike other programmers, it doesn't require a separate power supply! Its power comes directly from the host's USB interface. Better still, thanks to its low power footprint, it can also be used to supply the targeted microcontroller circuit
* It has a writing speed of max. 1Kb/s and a reading speed of max. 2Kb/s, being able to program [*ISP*](http://en.wikipedia.org/wiki/In-System_Programming) devices that have up to 64K of program memory
* It comes with 2 status LEDs: one that shows that the USB link is working and another one that shows any current upload/download progress
* It is both supported by Linux and Windows OSes ([*driver*](http://en.wikipedia.org/wiki/Device_driver)) and free supporting application ([*avrdude*](http://www.nongnu.org/avrdude/))
* Its [*firmware*](http://en.wikipedia.org/wiki/Firmware) is made available under an *open-source* license of [*GPL*](http://www.gnu.org/copyleft/gpl.html) type, while the board design is distributed under the [*Creative Commons Lincense : BY - SA*](http://creativecommons.org/)
* Its a perfect circuit to be built by students and/or hobbyists alike

All these features have made the *USBTinyISP* programmer a perfect candidate for the central module of _pAle_.
**The board could not be made possible without this programmer,** that's for sure! 

### A small footnote ###
I thank the guys and gals from [**ladyada**](http://www.ladyada.net) for making *USBTinyISP* public and available. [*Dick Streefland*](http://dicks.home.xs4all.nl/) is the main author of this wonderful programmer!  
You can find the original project page [here](http://dicks.home.xs4all.nl/avr/usbtiny/).
