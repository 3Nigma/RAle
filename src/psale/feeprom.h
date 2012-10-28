/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */
 
#ifndef _FEEPROM_H_
#define _FEEPROM_H_

#include <gtk/gtk.h>

#include "al.h"

#define FME_CITESTE_EEPROM 1
#define FME_SCRIE_EEPROM   2
#define FME_PARASESTE_FRM  3

typedef struct {
  GtkWidget *frm;
  GtkWidget *tvEEPROM;
} FormularEEPROM;

extern FormularEEPROM *
fme_initializeaza(GtkWindow *parinte);

extern void 
fme_afiseaza(FormularEEPROM *fe);

#endif
