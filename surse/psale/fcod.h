#ifndef _FCOD_H_
#define _FCOD_H_

#include <gtk/gtk.h>

typedef enum {C, ASM} Limbaj;
typedef enum {ASCUNSE, VIZIBILE} VizActiuni;

typedef struct {
  GtkWidget *frm;
  GtkWidget *btExpandator;
  GtkWidget *cadruActiuni;
  GtkWidget *lblStareConex, *lblStareDCod, *lblStareNSursa;
  VizActiuni vActiuni;
} FormularCod;

extern FormularCod *
fc_initializeaza(Limbaj lmDorit, const unsigned char *codInitial, gboolean esteExemplu);

extern FormularCod *
fc_initializeaza_fara_cod(Limbaj lmDorit, gboolean esteExemplu);

extern void 
fc_modifica_vizibilitate(FormularCod *fc, gboolean vizibil);

#endif
