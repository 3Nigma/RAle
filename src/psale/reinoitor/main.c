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

#include "db.h"
#include "dl.h"

int main(int argc, char *argv[]) {
  gtk_init(&argc, &argv);
  
  if(db_initializeaza()) {
	
	if(db_obtine_este_prima_rulare()) {
	  GtkWidget *frmPrimaIntrebare = NULL;
	  GtkWidget *zonaContinutFrmPrimIntr = NULL;
	  GtkWidget *lblInformatii = NULL;
	  
	  frmPrimaIntrebare = gtk_message_dialog_new_with_markup(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, 
                                                  GTK_BUTTONS_NONE, "Având în vedere că asta este prima dată când deschideți aplicația,\n\n"
                                                "<b>cum doriți să se realizeze actualizările ?</b>\n\n"
                                                "<i> > <b>În mod automat</b>: de fiecare dată când deschideți programul,\n"
                                                " > <b>Manual</b>: prin butonul de pe formularul de informații, sau\n"
                                                " > <b>Răspund mai târziu</b>: mă mai gândesc.</i>");
	  gtk_window_set_title(GTK_WINDOW(frmPrimaIntrebare), "Întrebare");
	  gtk_dialog_add_buttons(GTK_DIALOG(frmPrimaIntrebare), "Răspund mai târziu", 0,
													  "Actualizează manual", 1,
													  "Actualizează automat", 2,
													  NULL);
	  gtk_dialog_set_default_response(GTK_DIALOG(frmPrimaIntrebare), 2);
      gtk_dialog_run(GTK_DIALOG(frmPrimaIntrebare));
	  gtk_main();
	}
	
	if(dl_initializeaza(db_obtine_adresa_actualizare())) {
    
      if(dl_exista_versiune_mai_buna_decat(db_obtine_versiune_curenta())) {
		/* s-a analizat și s-a găsit o actualizare */
	    dl_actualizeaza_aplicatia();
	  }
      dl_curata();
    }
    
    db_curata();
  }
  
  /* TODO: lansează 'psAle' */
  
  return 0;
}
