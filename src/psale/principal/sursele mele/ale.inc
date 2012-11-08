; Autor : Victor ADĂSCĂLIȚEI @ 2012

; Fișierul curent conține declarații utile pentru creierașul plăcuței Ale v0.9, așa cum este ea achiziționată.
; Subrutine prezente aici împreună rolul și modul lor de apel :
;   -> afiseaza_r16      : afișează pe LED-urile plăcuței valoarea ce este prezentă în registrul general R16
;   -> citeste_taste_r17 : surprinde starea butonașelor plăcuței și o depozitează în registrul R17
;   -> intarzie_un_pic   : introdu o întârziere sensibilă în punctul de apel

; ######### Început zonă de declarare a regiștrilor speciali ###########
SREG   = 0x3F
SP     = 0x3D
GIMSK  = 0x3B
GIFR   = 0x3A
TIMSK  = 0x39
TIFR   = 0x38
SPMCSR = 0x37
MCUCR  = 0x35
MCUSR  = 0x34
TCCR0B = 0x33
TCNT0  = 0x32
OSCCAL = 0x31
TCCR1  = 0x30
TCNT1  = 0x2F
OCR1A  = 0x2E
OCR1C  = 0x2D
GTCCR  = 0x2C
OCR1B  = 0x2B
TCCR0A = 0x2A
OCR0A  = 0x29
OCR0B  = 0x28
PLLCSR = 0x27
CLKPR  = 0x26
DT1A   = 0x25
DT1B   = 0x24
DTPS1  = 0x23
DWDR   = 0x22
WDTCR  = 0x21
PRR    = 0x20
EEARH  = 0x1F
EEARL  = 0x1E
EEDR   = 0x1D
EECR   = 0x1C
PORTB  = 0x18
DDRB   = 0x17
PINB   = 0x16
PCMSK  = 0x15
DIDR0  = 0x14
GPIOR2 = 0x13
GPIOR1 = 0x12
GPIOR0 = 0x11
USIBR  = 0x10
USIDR  = 0x0F
USISR  = 0x0E
USICR  = 0x0D
ACSR   = 0x08
ADMUX  = 0x07
ADCSRA = 0x06
ADCH   = 0x05
ADCL   = 0x04
ADCSRB = 0x03
; ######### Sfârșit zonă de declarare a regiștrilor speciali ###########

; # Început zonă de declarare a biților prezenți în regiștri speciali ##
; biți SREG
I        = 7
T        = 6
H        = 5
S        = 4
V        = 3
N        = 2
Z        = 1
C        = 0

; biți SP
; ---

; biți GIMSK
INT0     = 6
PCIE     = 5

; biți GIFR
INTF0    = 6
PCIF     = 5

; biți TIMSK
OCIE1A   = 6
OCIE1B   = 5
OCIE0A   = 4
OCIE0B   = 3
TOIE1    = 2
TOIE0    = 1

; biți TIFR
OCF1A    = 6
OCF1B    = 5
OCF0A    = 4
OCF0B    = 3
TOV1     = 2
TOV0     = 1

; biți SPMCSR
RSIG     = 5
CTPB     = 4
RFLB     = 3
PGWRT    = 2
PGERS    = 1
SPMEN    = 0

; biți MCUCR
BODS     = 7
PUD      = 6
SE       = 5
SM1      = 4
SM0      = 3
BODSE    = 2
ISC01    = 1
ISC00    = 0

; biți MCUSR
WDRF     = 3
BORF     = 2
EXTRF    = 1
PORF     = 0

; biți TCCR0B
FOC0A    = 7
FOC0B    = 6
WGM02    = 3
CS02     = 2
CS01     = 1
CS00     = 0

; biți TCNT0
; ---

; biți OSCCAL
; ---

; biți TCCR1
CTC1     = 7
PWM1A    = 6
COM1A1   = 5
COM1A0   = 4
CS13     = 3
CS12     = 2
CS11     = 1
CS10     = 0

; biți TCNT1
; ---

; biți OCR1A
; ---

; biți OCR1C
; ---

; biți GTCCR
TSM      = 7
PWM1B    = 6
COM1B1   = 5
COM1B0   = 4
FOC1B    = 3
FOC1A    = 2
PSR1     = 1
PSR0     = 0

; biți OCR1B
; ---

; biți TCCR0A
COM0A1   = 7
COM0A0   = 6
COM0B1   = 5
COM0B0   = 4
WGM01    = 1
WGM00    = 0

; biți OCR0A
; ---

; biți OCR0B
; ---

; biți PLLCSR
LSM      = 7
PCKE     = 2
PLLE     = 1
PLOCK    = 0

; biți CLKPR
CLKPCE   = 7
CLKPS3   = 3
CLKPS2   = 2
CLKPS1   = 1
CLKPS0   = 0

; biți DT1A
DT1AH3   = 7
DT1AH2   = 6
DT1AH1   = 5
DT1AH0   = 4
DT1AL3   = 3
DT1AL2   = 2
DT1AL1   = 1
DT1AL0   = 0

; biți DT1B
DT1BH3   = 7
DT1BH2   = 6
DT1BH1   = 5
DT1BH0   = 4
DT1BL3   = 3
DT1BL2   = 2
DT1BL1   = 1
DT1BL0   = 0

; biți DTPS1
DTPS11   = 1
DTPS10   = 0

; biți DWDR
; ---

; biți WDTCR
WDIF     = 7
WDIE     = 6
WDP3     = 5
WDCE     = 4
WDE      = 3
WDP2     = 2
WDP1     = 1
WDP0     = 0

; biți PRR
PRTIM1   = 3
PRTIM0   = 2
PRUSI    = 1
PRADC    = 0

; biți EEARH
EEAR8    = 0

; biți EEARL
EEAR7    = 7
EEAR6    = 6
EEAR5    = 5
EEAR4    = 4
EEAR3    = 3
EEAR2    = 2
EEAR1    = 1
EEAR0    = 0

; biți EEDR
; ---

; biți EECR
EEPM1    = 5
EEPM0    = 4
EERIE    = 3
EEMPE    = 2
EEPE     = 1
EERE     = 0

; biți PORTB
PORTB5   = 5
PORTB4   = 4
PORTB3   = 3
PORTB2   = 2
PORTB1   = 1
PORTB0   = 0

; biți DDRB
DDB5     = 5
DDB4     = 4
DDB3     = 3
DDB2     = 2
DDB1     = 1
DDB0     = 0

; biți PINB
PINB5    = 5
PINB4    = 4
PINB3    = 3
PINB2    = 2
PINB1    = 1
PINB0    = 0

; biți PCMSK
PCINT5   = 5
PCINT4   = 4
PCINT3   = 3
PCINT2   = 2
PCINT1   = 1
PCINT0   = 0

; biți DIDR0 [indexi sunt în această ordine!]
ADC0D    = 5
ADC2D    = 4
ADC3D    = 3
ADC1D    = 2
AIN1D    = 1
AIN0D    = 0

; biți GPIOR2
; ---

; biți GPIOR1
; ---

; biți GPIOR0
; ---

; biți USIBR
; ---

; biți USIDR
; ---

; biți USISR
USISIF   = 7
USIOIF   = 6
USIPF    = 5
USIDC    = 4
USICNT3  = 3
USICNT2  = 2
USICNT1  = 1
USICNT   = 0

; biți USICR
USISIE   = 7
USIOIE   = 6
USIWM1   = 5
USIWM0   = 4
USICS1   = 3
USICS0   = 2
USICLK   = 1
USITC    = 0

; biți ACSR
ACD      = 7
ACBG     = 6
ACO      = 5
ACI      = 4
ACIE     = 3
ACIS1    = 1
ACIS0    = 0

; biți ADMUX
REFS1    = 7
REFS0    = 6
ADLAR    = 5
REFS2    = 4
MUX3     = 3
MUX2     = 2
MUX1     = 1
MUX0     = 0

; biți ADCSRA
ADEN     = 7
ADSC     = 6
ADATE    = 5
ADIF     = 4
ADIE     = 3
ADPS2    = 2
ADPS1    = 1
ADPS0    = 0

; biți ADCH
; ---

; biți ADCL
; ---

; biți ADCSRB
BIN      = 7
ACME     = 6
IPR      = 5
ADTS2    = 2
ADTS1    = 1
ADTS0    = 0
; ## Sârșit zonă de declarare a biților prezenți în regiștri speciali ##

; ############### Început zonă de declarare speciale ###################
; corespondență GPIO <-> pini fizici
PB5      = 5
PB4      = 4
PB3      = 3
PB2      = 2
PB1      = 1
PB0      = 0
; ############### Sfârșit zonă de declarare speciale ###################

; ##################### Început 'afiseaza_r16' #########################
afiseaza_r16:
 push R17
 push R16

 ldi R17, 8
 ciclu_afisare:
   sbrc R16, 7
   sbi PORTB, PB1
   sbrs R16, 7
   cbi PORTB, PB1

   sbi PORTB, PB0
   nop
   cbi PORTB, PB0

   lsl R16

   dec R17  
   cpi R17, 0
   brne ciclu_afisare  

 pop R16
 pop R17
 ret
; ##################### Sfârșit 'afiseaza_r16' #########################

; ################### Început 'citeste_taste_r17' ######################
citeste_taste_r17:
 push R16

 cbi PORTB, PB1
 cbi PORTB, PB4
 nop
 sbi PORTB, PB4

 clr R17
 ldi R16, 8
 ciclu_citire_taste:
   lsl R17
   sbis PINB, PINB2 
   ori R17, 0b00000001

   cbi PORTB, PB1
   nop
   sbi PORTB, PB1

   dec R16
   cpi R16, 0
   brne ciclu_citire_taste

 pop R16
 ret
; ################### Sfârșit 'citeste_taste_r17' ######################

; #################### Început 'intarzie_un_pic' #######################
intarzie_un_pic:
 push R27
 push R26
 push R25

 ldi R25, 3
 _ciclu_intarziere_M:
   ldi R27, 0xFF
   ldi R26, 0xFF
   _ciclu_intarziere:
     sbiw R26, 1
     brne _ciclu_intarziere
   subi R25, 1
   brne _ciclu_intarziere_M

 pop R25
 pop R26
 pop R27
 ret
; #################### Sfârșit 'intarzie_un_pic' #######################
