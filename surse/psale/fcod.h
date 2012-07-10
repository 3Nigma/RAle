#ifndef _FCOD_H_
#define _FCOD_H_

#include <gtk/gtk.h>

typedef enum {C, ASM} Limbaj;
typedef enum {ASCUNSE, VIZIBILE} VizActiuni;

typedef struct {
  GtkWidget *frm;
  GtkWidget *btExpandator;
  GtkWidget *cadruActiuni;
  VizActiuni vActiuni;
} FormularCod;

extern FormularCod *
fc_initializeaza(Limbaj lmDorit, gchar *codInitial, gboolean esteExemplu);

extern void 
fc_modifica_vizibilitate(FormularCod *fc, gboolean vizibil);

#endif
