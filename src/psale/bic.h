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
