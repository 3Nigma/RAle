/* Autor : Victor ADĂSCĂLIȚEI @ 2012
 * 
 * Fișierul de față conține declarații utile pentru creierașul plăcuței Ale v0.9, așa cum este ea achiziționată.
 * În continuare aveți o listă a funcțiilor prezente aici împreună cu rolul și modul lor de apel :
 *  -> void initializeazaAle() : configurează uC-ul pentru a lucra atât cu afișorul cât și cu minitastatura
 *  -> void afiseaza(uint8_t)  : afișează pe LED-urile plăcuței valoarea singurului său argument
 *  -> uint8_t citesteTaste()  : surprinde și returnează starea butonașelor plăcuței
*/

#include <avr/io.h>

void initializeazaAle(void) {
  DDRB = _BV(PB0) | _BV(PB1) | /* activează afișorul */
         _BV(PB4);  /* activează minitastatura */
}

void afiseaza(uint8_t octetDeAfisat) {
  uint8_t ct;

  /* PB0 - pinul de ceas pentru 74HC164 - output */
  /* PB1 - pinul de date pentru 74HC164 - output */

  /* trimite biții către circuit pentru afișare */
  for(ct = 0; ct < 8; ct++)  {
    if((octetDeAfisat & 0x80) == 0x80) PORTB |= _BV(PB1);
    else PORTB &= ~_BV(PB1);

    /* transferă bit către uC */
    PORTB |=_BV(PB0);
    PORTB &=~_BV(PB0);

    octetDeAfisat = octetDeAfisat << 1;
  }
 
  /* adu pinul de date la o valoare cunoscută */
  PORTB &= ~_BV(PB1);
}

uint8_t citesteTaste(void) {
  uint8_t tastaCitita = 0;
  int ct;
 
  /* PB1 - pinul de ceas pentru 74HC165 - output */
  /* PB2 - pinul de date pentru 74HC165 - input */
  /* PB4 - pinul de încărcare de taste în regiștrii interni ai circuitului 74HC165 - output */
 
  PORTB &= ~_BV(PB1);
  PORTB &= ~_BV(PB4);
  PORTB |= _BV(PB4);
 
  /* scoate valoarea butonașelor în mod serial (unul după altul) din circuit */
  for(ct = 0; ct < 8; ct++) {
    tastaCitita = tastaCitita >> 1;
    if(PINB & _BV(PINB2)) tastaCitita = tastaCitita | 0b10000000;
  
    PORTB &= ~_BV(PB1);
    PORTB |= _BV(PB1);
  }

  return ~tastaCitita;
}
