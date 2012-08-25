## Mai întâi o imagine ... ##
![Interacțiune 'module' <-> 'cod sursă'](http://i50.tinypic.com/2u8z61w.png)

## Apoi niște explicații ##
Pe latură de aplicații, găsiți în acest director trei direcții de lucru :
* _psAle_ : aplicația principală de interacțiune cu plăcuța pachetului,
* _dafus_ : aplicația (_firmware-ul_) programatorului ce se găsește pe plăcuța Ale,
* _acad_  : modulul de legătură între programul _psAle_ și plăcuță.

Toate sunt scrise preponderent în limbajul de programare C, numai _dafus_ are unele secțiuni scrise direct în limbaj mașină.

_psAle_ reprezintă aplicația centrală prin care utilizatorul pachetului educațional Ale poate să '_comunice_' cu plăcuța. Ea a fost dezvoltată integral și special pentru acest scop. Practic, aplicația permite realizarea următoarelor acțiuni :
* Scrierea și compilarea de cod propriu atât în limbajul C cât și în limbaj nativ de asamblare folosind un editor cu recunoaștere de sintaxă,
* Trimiterea către micro-creierașul plăcuței a codului dorit,
* Posibilitatea citirii cărțulii electronice într-o manieră simplă și '_aerisită_' care facilitează asimilarea de cunoștințe,
* Încărcarea și rularea exemplelor prezente în cărțulie cu inspectarea rezultatului imediat pe plăcuță,

Pentru a face toate aceste lucruri, _psAle_ are nevoie la rândul ei de o serie de aplicații **gratuite** (compilatorul [Avr-gcc](http://sourceforge.net/projects/winavr/files/WinAVR/) și programatorul [avrdude](http://www.nongnu.org/avrdude/)) care se instalează automat cu programul principal. Trebuie specificat faptul că instalatorul nu este încă implementat. O variantă bună în această direcție ar fi sistemul de instalare [**Nullsoft** _NSIS_](http://nsis.sourceforge.net/Main_Page). 

Mergând mai departe, _acad_ se dorește a fi o librărie minimală de interacțiune a plăcuței cu _psAle_ care să ia locul aplicației [AvrDude](http://www.nongnu.org/avrdude/). Totodată trebuie menționat faptul că _acad_, care vine din prescurtarea "**Ale's Custom Avrdude Distro'**" (în traducere : "O distribuție modificată a aplicației Avrdude pentru Ale"], a izvorât din codul sursă a proiectului "**AvrDude**".  
Motivul pentru care se caută un înlocuitor pentru acest nivel de interacțiune este unul de control. Se dorește ca această librărie să pună la dispoziție o serie de funcționalități pe care doar plăcuța Ale le înțelege. 
Deocamdată _psAle_ se folosește de aplicația originală _AvrDude_, librăria fiind în curs de dezvoltare.

Pe lângă costul său redus (aplicația este chiar **gratuită**), un alt motiv pentru care este folosit _AvrDude_ o reprezintă capacitatea sa de a comunica cu programatorul [**USBtinyISP**](http://www.ladyada.net/make/usbtinyisp/). Acest tip de programator este folosit și de către plăcuța Ale, prin urmare decizia de folosire a aplicației a fost una corectă.  
_dafus_, cel de-a treia ramură a codului sursă, își propune să personalizeze codul sursă al programatorului *USBtinyISP* pentru ca, împreună cu _acad_ să confere mai multe funcționalități plăcuței.
Asemeni lui _acad_, _dafus_ are ca bază de pornire codul sursă folosit de proiectul **USBtinyISP**.

Ca să vă faceți o idee, una din direcțiile de dezvoltare a celor trei aplicații mai departe decât baza de pornire a proiectelor constituente ar fi capacitatea plăcuței de a comunica cu _psAle_ nu numai în materie de programare, dar și în materie de mesaje proprii scrise de utilizator. Ori este evident că pentru a face acest lucru posibil atât aplicația programatorului (**USBtinyISP** prin extensia locală _dafus_) cât și aplicația ce asigură programarea din PC (**AvrDude** prin versiunea proprie _acad_) și mai ales _psAle_ trebuiesc modificate.

### Indicații de construire a aplicației ###
[vor urma printr-o referire la o pagină wiki]

## + drepturi legale ##
Având trei miniproiecte, avem trei licențe :
* _psAle_ se distribuie sub o licență [Creative Commons](http://creativecommons.org/) de tip !['BY - NC - SA' logo](http://i.creativecommons.org/l/by-nc-sa/3.0/88x31.png). Mai multe detalii găsiți (**în română**) [aici](http://creativecommons.org/licenses/by-nc-sa/3.0/ro/),
* _dafus_, trăgându-se din [**USBtinyISP**](http://www.ladyada.net/make/usbtinyisp/), îi moștenește licența [GPL](http://ro.wikipedia.org/wiki/GPL_%28licen%C8%9B%C4%83,_versiunea_3%29) _și_
* _acad_, un descendent a lui [**AvrDude**](), i se atribuie aceeași licență de [GPL v3](http://ro.wikipedia.org/wiki/GPL_%28licen%C8%9B%C4%83,_versiunea_3%29).

În final, țin să mulțumesc autorilor imaginilor '_împrumutate_' pentru realizarea acestui document : 
* Setul [Electronic 3D](http://www.iconarchive.com/show/electronics-icons-by-double-j-design.html) de [Double-J Design](http://www.iconarchive.com/artist/double-j-design.html),
* Setul [Pretty Office 5 Icons](http://www.iconarchive.com/show/pretty-office-5-icons-by-custom-icon-design.html) a lui [Custom Icon Design](http://www.iconarchive.com/artist/custom-icon-design.html) _și_
* Setul [SimpleGreen Icons](http://www.iconarchive.com/show/simple-green-icons-by-simplefly.html) realizat de [Simplefly](http://www.iconarchive.com/artist/simplefly.html).
