/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#ifndef _FCOD_H_
#define _FCOD_H_

#include <gtk/gtk.h>

#include "bic.h"

typedef enum {C, ASM} Limbaj;
typedef enum {ASCUNSE, VIZIBILE} VizActiuni;
typedef enum {DESCHIDE, SALVEAZA} TipDialogFisier;

typedef struct {
  GtkWidget *frm;
  Limbaj lmFolosit;
  GtkWidget *txtVizCod;
  gboolean esteExempluIncarcat;
  gchar numeSimpluAfisat[128];
  gchar caleCurentaSursa[256];
  GtkWidget *btExpandator;
  GtkWidget *cadruActiuni;
  GtkWidget *lblStareConex, *lblStareDCod, *lblStareNSursa;
  BaraInfoCod bInfo;
  VizActiuni vActiuni;
  gpointer parinteGazda;
  
  void (*laSchimbare_starePlacuta_recurenta)(gpointer, gboolean);
} FormularCod;

extern FormularCod *fc_initializeaza_fara_cod(gpointer parinteStructura, Limbaj lmDorit);

extern FormularCod *fc_initializeaza_cu_exemplu(gpointer parinteStructura, const gchar *titluScurt, const gchar *titluLung);

extern void fc_actualizeaza_stare_placuta(GtkWidget *lblStare, gboolean online, gboolean primaRulare);

extern void fc_modifica_vizibilitate(FormularCod *fc, gboolean vizibil);

#endif
