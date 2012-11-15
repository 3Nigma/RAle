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
#include <glib/gstdio.h>

#include "fprin.h"

#define ACTUALIZATOR_NUME_ACTUALIZAT "rpsale.nou"
#ifdef G_OS_WIN32
  #define ACTUALIZATOR_NUME_AP "rpsale.exe"
#elif defined G_OS_UNIX
  #define ACTUALIZATOR_NUME_AP "rpsale"
#endif

static gboolean
incearca_actualizare_actualizator() {
  gboolean rezultatOp = FALSE;
  
  if(g_file_test(ACTUALIZATOR_NUME_ACTUALIZAT, G_FILE_TEST_EXISTS)) {
    /* există o versiune mai nouă a acutalizatorului descărcată.
     * Înlocuiește actualizatorul curent cu cel nou */
     if(g_remove(ACTUALIZATOR_NUME_AP) == 0) {
	   if(g_rename(ACTUALIZATOR_NUME_ACTUALIZAT, ACTUALIZATOR_NUME_AP) == 0) {
	     rezultatOp = TRUE;
	   }
	 }
  }

  return rezultatOp;
}

int main(int argc, char *argv[]) {
  FormularPrincipal *fprin = NULL;

  if(incearca_actualizare_actualizator()) {
    g_debug("Am realizat cu succes înlocuirea actualizatorului cu o versiune mai nouă.");
  }
  
  gtk_init (&argc, &argv);
  
  fprin = fp_initializeaza_formular_principal();
  fp_arata(fprin);
  
  gtk_main();
  fp_curata(fprin);
  return 0;
}
