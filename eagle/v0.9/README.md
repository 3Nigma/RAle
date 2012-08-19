## O primă vedere ##
Plăcuța *pachetului Ale* are următoarea dispunere modulară a funcționalităților sale :
<center>![Diagrama Funcțională a Plăcuței](http://i45.tinypic.com/29cq7q0.png)</center>

## Acum că suntem curioși ##
Dintre toate fișierele ce se găsesc în acest director, doar două au o importanță deosebită pentru proiect. Este vorba despre fișierele

* **v0.9.sch** care prezintă dispunerea logică a componentelor *și*
* **v0.9.brd** care arată dispunerea fizică a pieselor

Ambele fișiere, *sch* și *brd*, se pot deschide cu aplicația [Eagle Electrical CAD](http://www.cadsoftusa.com/) care are și *o versiune gratuită*. Ele reprezintă modelul electric și fizic de construcție al plăcuței Ale versiunea 0.9. Este vorba de una din cele 3 componente ale pachetului educațional prin care cititorul poate exersa cunoștințele dobândite prin cărțulie și aplicate prin programul *psAle*.

În continuare vom pătrunde un pic mai în amănunt și vom puncta capacitățile plăcuței.

### Facilități ale plăcuței ###
* Are la bază programatorul [USBTinyISP versiunea 2.0](http://www.ladyada.net/make/usbtinyisp/) prin care scrierea aplicațiilor pe **creieraș** devine posibilă,
* Grație programatorului utilizat, plăcuța nu necesită alimentare externă, iar conexiunea cu PC-ul se face într-un fel modern, prin intermediul unui cablu USB,
* Este gândită pentru a lucra împreună cu microcontrollere de tip **Attiny[13|25|45|85]**. Materialul informativ lucrează cu *Attiny25*, dar în locul acestuia se poate utiliza orice componentă din cele amintite în detrimentul unor mici incompatibilități. Un exemplu de astfel de incompatibilitate ar fi senzorul intern de temperatură pe care numai Attiny25-ul îl are. Acest fapt face ca secțiunea din cărțulie dedicată acestui subiect să nu se mai aplice în cazul folosirii altui microcontroller decât a celui indicat,
* Are un butonaș de resetare prin care programatorul poate să readucă execuția aplicației din creieraș la o stare inițială cunoscută,
* Prezintă un element fotosensibil prin care *se poate citi* nivelul luminii din exteriorul plăcuței,
* Este prevăzută cu 8 LED-uri prin care starea internă a microcontrollerului poate fi expusă lumii înconjurătoare,
* Dispune de un de o minitastatură cu 8 butonașe prin care se pot trimite valori microcontrollerului,
* *Attiny*-ul în cauză, dar și oricare din microcontrollere amintite, dispune de facilități interne numite *module* prin care se pot realiza aplicații dintre cele mai complexe.

## Și restul de fișiere ? ##
... sunt o *consecință* a apartenenței plăcuței la *licența gratuită* de tip [CERN OHL v1.1]( http://www.ohwr.org/cernohl/). Datorită faptului că licența este concepută în limba engleză, următoarele fișiere care sunt folosite de această licență sunt scrise de asemenea în această limbă. Acestea fișiere sunt :

* **LICENSE.pdf** care conține textul integral al licenței alese, de la facilități la responsabilități,
* **HOWTO.pdf** - prezintă informații mai ușoare despre licență și cum se poate folosi acest tip în alte circuite electrice,
* **CHANCES.TXT** - fișierul se va completa cu modificările realizate de alte persoane care doresc să folosească plăcuța,
* **CONTRIB.TXT** - în el se găsește o listă de adrese de email care trebuiesc înștiințate atunci când cineva modifică documentația plăcuței,
* **PRODUCT.TXT** - conține o listă de emailuri ce trebuiesc înștiințate dacă se dorește reproducerea fizică a plăcuței.

Pentru mai multe informații cu privire la toate aceste fișiere, descărcați și citiți documentul **HOWTO.pdf**.
