/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#ifndef _FINFO_H_
#define _FINFO_H_

#include "sda.h"
#include <gtk/gtk.h>

typedef struct {
    GtkWidget *frm;
    GtkWidget *txtInfoVersiuni;
    GtkWidget *btActualizeaza;
    GtkWidget *btParasesteFrm;
    GtkWidget *lblStareActualizare;
    GtkWidget *imgStareActualizare;
} FInfoInstanta;

typedef enum {
    FI_ACTUALIZARE_INITIALIZARE,
    FI_ACTUALIZARE_IN_CURS,
    FI_ACTUALIZARE_SUCCES_VERSNOUA,
    FI_ACTUALIZARE_SUCCES_VERSNESCHIMBATA,
    FI_ACTUALIZARE_ESEC,
} FIStareActualizare;

#define FI_IMG_STARE_ACTUALIZARE_LUNGIME 16
#define FI_IMG_STARE_ACTUALIZARE_INALTIME 16
#define FI_ACTUALIZARE_URI_PORNESTE "actualizeaza_manual"
#define FI_ACTUALIZARE_URI_ACTUALIZEAZA "actualizeaza_direct"

extern FInfoInstanta *finfo_initializeaza(GtkWindow *parinte);
extern void finfo_curata(FInfoInstanta **fii);

extern void finfo_arata(FInfoInstanta *fii);

#endif
