/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

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
