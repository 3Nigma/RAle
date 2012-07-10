#ifndef _FCOD_H_
#define _FCOD_H_

#include <gtk/gtk.h>

typedef enum{C, ASM} Limbaj;

extern GtkWidget *
initializeaza_formular_cod(Limbaj lmDorit, gchar *codInitial, gboolean esteExemplu);

#endif
