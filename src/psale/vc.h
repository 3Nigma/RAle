/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#ifndef _VC_H_
#define _VC_H_

#include <gtk/gtk.h>
#include <poppler.h>

#define TEXT_FACTOR_TITLU ".electro:cărțulia"
#define SCROLURI_PER_PAG 20
#define INCREMENT_MARIRE 0.05
#define PAGINA_CUPRINS 5
#define MAX_INCR_ZOOM 1.9
#define MIN_INCR_ZOOM 0.5
#define X_IMG_SCAL_CUPRINS 1.0
#define Y_IMG_SCAL_CUPRINS 1.0

typedef struct {
  GtkWidget *frm;
  GtkWidget *pdfviz;
  GtkAdjustment *pdfScrolBVertical;
  PopplerDocument *doc;
  cairo_surface_t *suprafataPag;
  
  GList *salturi_curente;
  PopplerAction *actiune_salt_proxim;
  
  gint x_start_rendare;
  gint y_start_rendare;
  double x_pag_scale;
  double y_pag_scale;
  double pag_height;
  double pag_width;
  gint nr_pag_curenta;
  gint nr_pag_totale;
} VizualizatorCartulie;

extern VizualizatorCartulie * 
vc_initializeaza();

extern void 
vc_sari_la_pagina(VizualizatorCartulie *vc, int nrPag);

#endif
