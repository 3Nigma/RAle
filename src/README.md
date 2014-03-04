## Let's start with an image ... ##
![Module <-> source code integration](http://i50.tinypic.com/2u8z61w.png)
###^- Image pending for internationalization :-)###

## ... and continue with some explanations ...##
This folder contains 3 working modules:
* _psAle_ : the main software used for interacting with the board
* _dafus_ : _pAle_ programmer firmware
* _acad_  : low level application driver that interfaces _psAle_ with _pAle_

All these parts are written mainly in ISO C. _dafus_ also has some regions written in assembly language.

_psAle_ is the main, custom written, application with which the learner can issue instructions that can be run on the _pAle_ board. Basically, the software allows the following actions to be carried out:
* Writing and compiling of both native assembly and embedded C applications using an integrated editor with syntax highlighting capability
* Easy 'dumping' of applications to the board
* Reading the electronic version of the book
* Elegant loading of the book examples with an immediate result inspection

To do all these things, _psAle_ needs to interact with a series of **free applications** -the [**Avr-gcc**](http://sourceforge.net/projects/winavr/files/WinAVR/) compiler and the [**AvrDude**](http://www.nongnu.org/avrdude/) programmer- which automatically get installed alongside it. The installer is created using the [**Nullsoft** _NSIS_](http://nsis.sourceforge.net/Main_Page) generator and takes care of everything: unpacking documentation, creation of shortcuts and even the driver installation. 

Next up, _acad_ is meant to be a minimal interaction library incapsulated within _psAle_ that will replace  [AvrDude](http://www.nongnu.org/avrdude/) alltogether. It's worth mentioning here the fact that _acad_, which stands for "**Ale's Custom Avrdude Distro'**", will be, at its core, a minimalist **AvrDude** compilation. 

The reason for planing to migrate to a monolithic software solution rather than relying on a proven 3rd part application has to do with its incomplete functionality considerations. We wish that, in due time, _acad_ will gather functionality that is custom for our cause.

For now, _psAle_ will continue using *AvrDude*, _acad_ not yet being available.

*AvrDude* is a truly *free* application with many features and generous list of supported programmers. [**USBtinyISP**](http://www.ladyada.net/make/usbtinyisp/), the programmer module that we also use in _pAle_, is one of such supported pieces of hardware.

On the other hand, _dafus_, the third and last code structure present here, plans to customize the firmware of the original *USBtinyISP* programmer used by the board as to bring more functionality to it (eg. application level communication between the host PC and the board).

Like _acad_, _dafus_ will use a modified version of the original **USBtinyISP** code.

Future work will focus on stabilizing these 3 subproject and extending them to include other features that can enrich the example and theory base of PEA.

### Support in building the applications ###
*[will follow in the wiki pages]*

## Legalities ##
Having 3 sub-projects, there are 3 licenses mentioned here :
* _psAle_ is made available under [Creative Commons](http://creativecommons.org/) type !['BY - NC - SA' logo](http://i.creativecommons.org/l/by-nc-sa/3.0/88x31.png). For more details, check [this](http://creativecommons.org/licenses/by-nc-sa/3.0/en/),
* _dafus_, comes from [**USBtinyISP**](http://www.ladyada.net/make/usbtinyisp/) inheriting its  [GPL](https://www.gnu.org/copyleft/gpl.html) license
* _acad_, a descendent of [**AvrDude**](http://www.nongnu.org/avrdude/) has its license as well: [GPL v3](https://www.gnu.org/copyleft/gpl.html).

In the end, I wish to thank the authors of the '_borrowed_' images that beautified this page: 
* [Electronic 3D](http://www.iconarchive.com/show/electronics-icons-by-double-j-design.html) set of [Double-J Design](http://www.iconarchive.com/artist/double-j-design.html),
* [Pretty Office 5 Icons](http://www.iconarchive.com/show/pretty-office-5-icons-by-custom-icon-design.html) set of [Custom Icon Design](http://www.iconarchive.com/artist/custom-icon-design.html)
* [SimpleGreen Icons](http://www.iconarchive.com/show/simple-green-icons-by-simplefly.html) set made by  [Simplefly](http://www.iconarchive.com/artist/simplefly.html).
