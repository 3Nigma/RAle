#include <gtk/gtk.h>

#include "dl.h"
#include "fprin.h"

int main(int argc, char *argv[]) {
  GtkWidget *formPrincipal = NULL;

  gtk_init (&argc, &argv);
  
  formPrincipal = fp_initializeaza_formular_principal();
  gtk_widget_show_all(formPrincipal);
  
  gtk_main();
  fp_curata();
  dl_curata();

  return 0;
}
