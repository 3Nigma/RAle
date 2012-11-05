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

#ifdef G_OS_WIN32
  #include <windows.h>
#elif defined G_OS_UNIX
  #include <string.h>
  #include <unistd.h>
  #include <errno.h>
#endif

#define RPS_NUME_PSALE "psale"
#define RPS_CALE_PSALE "./"RPS_NUME_PSALE

#define RPS_PRIMA_INTREB_RASPUNS_MAI_TARZIU 0
#define RPS_PRIMA_INTREB_RASPUNS_MANUAL     1
#define RPS_PRIMA_INTREB_RASPUNS_AUTOMAT    2

static void frmPrimaIntrebare_click_raspuns(GtkDialog *dialog, gint idRaspuns, gpointer user_data);
static void lanseazaPSAle();

static void 
frmPrimaIntrebare_click_raspuns(GtkDialog *dialog, gint idRaspuns, gpointer user_data) {
  switch(idRaspuns) {
  case GTK_RESPONSE_DELETE_EVENT:
  case RPS_PRIMA_INTREB_RASPUNS_MAI_TARZIU:
    break;
  case RPS_PRIMA_INTREB_RASPUNS_MANUAL:
    db_seteaza_actualizare_manuala();
    db_consuma_prima_rulare();
    break;
  case RPS_PRIMA_INTREB_RASPUNS_AUTOMAT:
    db_seteaza_actualizare_automata();
    db_consuma_prima_rulare();
    break;
  }
  
  gtk_widget_destroy(GTK_WIDGET(dialog));
  gtk_main_quit();
}

int main(int argc, char *argv[]) {
  gtk_init(&argc, &argv);
  
  if(db_initializeaza()) {
	
	if(db_obtine_este_prima_rulare()) {
	  GtkWidget *frmPrimaIntrebare = NULL;
	  
	  /* parese că este prima execuție a aplicației (de la instalare ?).
	   * Interoghează-l pe utilizator privind modul de actualizare dorit */
	  frmPrimaIntrebare = gtk_message_dialog_new_with_markup(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, 
                                                  GTK_BUTTONS_NONE, "Având în vedere că aceasta este prima dată când deschideți aplicația,\n\n"
                                                "<b>cum doriți să se realizeze actualizările ?</b>\n\n"
                                                "<i> > <b>În mod automat</b>: la fiecare pornire a aplicației,\n"
                                                " > <b>Manual</b>: prin butonul de pe formularul de informații, sau\n"
                                                " > <b>Răspund mai târziu</b>: mă mai gândesc.</i>");
	  gtk_window_set_title(GTK_WINDOW(frmPrimaIntrebare), "Întrebare");
	  gtk_window_set_position(GTK_WINDOW(frmPrimaIntrebare), GTK_WIN_POS_CENTER);
	  gtk_dialog_add_buttons(GTK_DIALOG(frmPrimaIntrebare), "Răspund mai târziu", RPS_PRIMA_INTREB_RASPUNS_MAI_TARZIU,
													  "Actualizează manual", RPS_PRIMA_INTREB_RASPUNS_MANUAL,
													  "Actualizează automat", RPS_PRIMA_INTREB_RASPUNS_AUTOMAT,
													  NULL);
	  gtk_dialog_set_default_response(GTK_DIALOG(frmPrimaIntrebare), RPS_PRIMA_INTREB_RASPUNS_AUTOMAT);
      gtk_widget_show_all(frmPrimaIntrebare);
      g_signal_connect(frmPrimaIntrebare, "response", G_CALLBACK(frmPrimaIntrebare_click_raspuns), NULL);
      
	  gtk_main();
	}
	
	if(db_obtine_este_actualizare_automata()) {
	  /* utilizatorul dorește actualizare automată ori de câte ori pornește aplicația */
	  if(dl_initializeaza(db_obtine_adresa_actualizare())) {
        /* modulul de descărcări (downloads : 'dl') a fost inițializat */
        if(dl_exista_versiune_mai_buna_decat(db_obtine_versiune_curenta())) {
	      /* s-a analizat și s-a găsit o versiune de aplicație mai bună. 
	       * Începe procedeul de actualizare */
	      dl_actualizeaza_aplicatia();
	    } else {
		  g_debug("Nu s-a găsit o versiune mai bună a aplicației.");
		}
        dl_curata();
      }
    }
    
    db_curata();
  }
  
  lanseazaPSAle();
  
  return 0;
}

static void 
lanseazaPSAle() {
#ifdef G_OS_WIN32
  STARTUPINFO startupInfo;
  PROCESS_INFORMATION processInformation;

  ZeroMemory(&startupInfo, sizeof(startupInfo));
  ZeroMemory(&processInformation, sizeof(processInformation));

  startupInfo.cb = sizeof(startupInfo);	

  if(!CreateProcess(NULL, RPS_CALE_PSALE, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &processInformation)) {
    g_error("Nu am putut porni aplicația 'psAle'!");
  }
#elif defined G_OS_UNIX
  pid_t IdProcCopil;
  
  if((IdProcCopil = fork()) != 0) {
    /* suntem în firul copil */
    if(execlp(RPS_CALE_PSALE, RPS_NUME_PSALE, NULL) == -1) {
	  g_error("Nu am putut porni aplicația 'psAle'! : %s", strerror(errno));
	}
  }
#endif
}
