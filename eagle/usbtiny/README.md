## Ce avem noi aici ? ##
![Conținut director 'RAle/eagle/usbtiny'](http://i45.tinypic.com/wtxi6r.png)

## De unde până unde ? ##
În acest director se găsesc fișierele electrice (circuitul logic - *sch* și amplasarea fizică a componentelor - *brd*) a proiectului [USBTinyISP versiune 2.0](http://www.ladyada.net/make/usbtinyisp/index.html).  
De pe saitul producătorului aflăm următoarele lucruri despre circuit :

* Este un programator *open-source/hardware* pentru microcontrollere de tip [*AVR*](http://en.wikipedia.org/wiki/Atmel_AVR) cu interfață de control de tip USB,
* Gândit ca și soluție **ieftină și rapidă** de programare a circuitelor inteligente de tip *AVR*,
* Spre deosebire de alte programatoare, nu necesită alimentare separată! Toată puterea de care are nevoie programatorul o extrage direct din legătura USB cu gazda. Mai mult, datorită consumului său redus, rămâne suficientă putere pentru a alimenta circuitul programat dacă acest lucru se dorește,
* Dispune de o viteză mare de scriere (max. 1Kb/s) și citire (maximum 2Kb/s) având capacitatea de a accesa circuite cu interfață de programare serială ([*ISP*](http://en.wikipedia.org/wiki/In-System_Programming)) ce dispun de până la 64KiloOcteți memorie de program,
* Are 2 LED-uri indicatoare: unul pentru a semnala faptul că legătura USB a fost stabilită și unul pentru a arăta progresul acțiunii curente,
* Are suport pentru sistemul de operare Linux și Windows ([*drivere*](http://en.wikipedia.org/wiki/Device_driver)) și aplicație gratuită ([*avrdude*](http://www.nongnu.org/avrdude/)) de transfer a programului conceput din calculator pe microcontroller-ul programat,
* Codul său sursă (poartă denumirea de [*firmware*](http://en.wikipedia.org/wiki/Firmware)) este distribuit sub o licență de tip *open-source*, mai specific de tip [*GPL*](http://www.gnu.org/copyleft/gpl.html), iar design-ul plăcuței fizice vine sub licență [*Creative Commons Lincense : BY - SA*](http://creativecommons.org/),
* Este un circuit ideal de construit de către studenți și/sau hobby-iști.

Toate aceste caracteristici au făcut din programatorul *USBTinyISP* candidatul ideal pe postul de modul central al plăcuței Ale.  
**Fără acest programator, plăcuța și chiar cărțulia ar fi literalmente inutile!** 

### O mică notă de subsol ###
Mulțumesc celor de la **ladyada** că au făcut public proiectul *USBTinyISP*, proiect inițial conceput de către [*Dick Streefland*](http://dicks.home.xs4all.nl/)!  
Pagina originală a proiectului o găsiți [aici](http://dicks.home.xs4all.nl/avr/usbtiny/).
