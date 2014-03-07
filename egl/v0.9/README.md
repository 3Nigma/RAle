## A first look ##
The PEA board (known as _pAle_) has the following, internal, modular composition:
<center>![Modular composition of pAle](http://i62.tinypic.com/hs4xvk.png)</center>

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

## The BOM list ##
Just to get a better understanding of the expenses involved in building the board, I give you the following *Bill Of Materials* table. The data was generated using Eagle's **bom.ulp** script.  
Therefore, as of this writing (version 0.9), the board required the following parts:

| Qty | Value      | Package      | Parts                                        |
| --- | ---------- | ------------ | -------------------------------------------- |
| 2   | 1.5k       | 0207/10      | R3, R8                                       |  
| 1   | 1.5k       | 0207/12      | R9                                           |
| 1   | 100nF      | C050-025X075 | C6                                           |
| 1   | 100u/25V   | E2,5-6       | C1                                           |  
| 3   | 10k        | 0204/7       | R7, R22, R23                                 |
| 7   | 10k        | 0207/10      | R6, R20, R21, R24, R25, R26, R27             |
| 1   | 10k        | 0207/15      | R11                                          |
| 1   | 12Mhz      | CSTLS_X      | X1                                           |
| 1   | 1k         | 0204/5       | R2                                           |
| 9   | 1k         | 0207/10      | R1, R12, R13, R14, R15, R16, R17, R18, R19   |
| 1   | 33         | 0207/10      | R5                                           |
| 1   | 33         | 0207/12      | R4                                           |
| 2   | 3v3        | ZDIO-7.5     | DZ1, DZ2                                     |
| 1   | 74164N     | DIL14        | V1                                           |
| 1   | 74165N     | DIL16        | V2                                           |
| 1   | 74HC125N   | DIL14        | IC1                                          |
| 1   | ATTINY2313 | DIL20        | IC3                                          |
| 1   | D1160_12   | D11XX        | FR35/1M                                      |
| 1   | Red        | LED3MM       | LED1                                         |
| 9   | TACT-64N   | B3F-10XX     | D0, D1, D2, D3, D4, D5, D6, D7, S1           |
| 1   | TINY25     |  DIL08       | IC2                                          |
| 1   | USBB-G     | PN61729      | X2                                           |
| 9   | Green      | LED3MM       | LED2, Q0, Q1, Q2, Q3, Q4, Q5, Q6, Q7         |

Please keep in mind the fact that if you plan to build the board yourself and you do have the know-how, the tools and the parts required, you will also need an Attiny2313 programmer to configure pAle's programmer module.

## What about the License? ##
Nothig to worry about! I just issued the board under the [Open Source Hardware (OSHA) License](http://www.oshwa.org/definition/) which means that you are free to hack it in any way you desire. Of course, I would very much appreciate to hear about your modifications.
