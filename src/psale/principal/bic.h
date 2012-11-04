/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#ifndef _BIC_H_
#define _BIC_H_

#include <gtk/gtk.h>

#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourceview.h>

typedef struct {
  GtkWidget *wid;
  GtkWidget *text;
  gboolean areLiniiUtile;
  gint nrLiniiUtileAfisate;
  
  GtkSourceBuffer *parentBuff;
  GtkSourceView *parentView;
  
  void (*cereRepozitionareaCursorului)(GtkSourceBuffer *bcod, GtkSourceView *vcod, gint linieNoua);
} BaraInfoCod;

extern void 
bic_initializeaza(BaraInfoCod *bicStructure);

extern void
bic_ascunde(BaraInfoCod *bicStructure);

extern void
bic_arata(BaraInfoCod *bicStructure);

extern void 
bic_text_initializeaza(BaraInfoCod *bicStructure);

extern void 
bic_text_adauga_linie(BaraInfoCod *bicStructure, const gchar *msg);

extern gboolean 
bic_text_contine_informatii_utile(BaraInfoCod *bicStructure);


#endif
