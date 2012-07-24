#ifndef _VC_H_
#define _VC_H_

#include <gtk/gtk.h>
#include <poppler.h>

#define SCROLURI_PER_PAG 20
#define INCREMENT_MARIRE 0.05
#define PAGINA_CUPRINS 5

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
