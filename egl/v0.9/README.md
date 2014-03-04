## A first look ##
The PEA board (known as _pAle_) has the following, internal, modular composition:
<center>![Modular composition of pAle](http://i45.tinypic.com/29cq7q0.png)</center>
###^- Image pending for internationalization :-)###

## Now that you are curious ##
From all the files listed in this folder, only 2 are very important for PEA. I'm talking here about:
* **v0.9.sch** which represents the schematic circuit itself
* **v0.9.brd** which reflects the physical layout of the components themselves

Both files (*sch* and *brd*) can be opened with the [Eagle Electrical CAD](http://www.cadsoftusa.com/) application which also has a *free version* available. These files represent the logical and physical construction specs for the _pAle_ board (version 0.9).

Let's go further and highlight some of the board's features.

### pAle's capabilities ###
* Built around the [USBTinyISP version 2.0](http://www.ladyada.net/make/usbtinyisp/) programmer, allowing the dumping of applications to Ale's little microcontroller
* Thanks to the programmer, the board does not require an external power supply and uses a modern data interface
* Is designed to work mainly with microcontrollers of type **Attiny[13|25|45|85]**. The original board uses an *Attiny25*, but it can also utilize other variants with little-or-no functional penalties. One such functional issue might be the internal temperature sensor which only the Attiny25 has. Under these circumstances, moving to a different microcontroller, this makes the book section devoted to this subject inapplicable
* It has a reset button which, as its name implies, resets the application that is currently running on the board
* Has a light-sensitive element through which one can read the level of lightning
* It has 8 general purpose LEDs, available to user applications
* It has 8 keys through which values from the environment can be sent to the user programs
* The chosen microcontroller family -*Attiny*- has many internal modules that can be used to develop complex projects

## What about the rest of the files? ##
... are a consequence of the license type -[CERN OHL v1.1]( http://www.ohwr.org/cernohl/)- chosen for the board. These files are:
* **LICENSE.pdf** - contains the full license text from rights to responsibilities
* **HOWTO.pdf** - contains some lightweighted information that indicate how one can make use of this license in other electrical circuits
* **CHANCES.TXT** - this file will be filled in by other persons who wish to use the board
* **CONTRIB.TXT** - contains a list of email addresses belonging to persons who must be notified when someone makes modifications to the circuit itself
* **PRODUCT.TXT** - contains a list of emails belonging to persons who must be notified when someone wishes to mass-produce the board (more then 10 pcs.)

For more information regarding these files, read the **HOWTO.pdf** file.
