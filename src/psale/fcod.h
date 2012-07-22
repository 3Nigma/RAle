#ifndef _FCOD_H_
#define _FCOD_H_

#include <gtk/gtk.h>

typedef enum {C, ASM} Limbaj;
typedef enum {ASCUNSE, VIZIBILE} VizActiuni;

typedef struct {
  GtkWidget *frm;
  Limbaj lmFolosit;
  GtkWidget *txtVizCod;
  GtkWidget *btExpandator;
  GtkWidget *cadruActiuni;
  GtkWidget *lblStareConex, *lblStareDCod, *lblStareNSursa;
  VizActiuni vActiuni;
  
  void (*laDepistare_neprezentaPlacuta_recurenta)();
} FormularCod;

extern FormularCod *
fc_initializeaza(Limbaj lmDorit, const char *codInitial, gchar *denumireSursa, gboolean esteExemplu);

extern FormularCod *
fc_initializeaza_fara_cod(Limbaj lmDorit);

extern void 
fc_actualizeaza_stare_placuta(GtkWidget *lblStare, gboolean online, gboolean primaRulare);

extern void 
fc_modifica_vizibilitate(FormularCod *fc, gboolean vizibil);

#endif
