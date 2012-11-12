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
#include <string.h>
#include <stdlib.h>

#include "db.h"
#include "dl.h"

#ifdef G_OS_WIN32
  #include <windows.h>
#elif defined G_OS_UNIX
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

IntrareActualizare *
reconstituieCerereDeActualizare(int argc, char *argv[]) {
  IntrareActualizare *iac = NULL;
  int cnt = 0;
  
  iac = (IntrareActualizare *)malloc(sizeof(IntrareActualizare));
  iac->adrPachetNou = (char *)calloc(1024, sizeof(char));
  iac->vers = 0.0;
  iac->mesajModificari = (char *)calloc(4096, sizeof(char));
  
  for(cnt = 1; cnt <= argc && argv[cnt] != NULL; cnt += 2) {
    if(strcmp(argv[cnt], "-a") == 0) {
      /* ar trebui să urmeze adresa pachetului în argv[cnt + 1] */
	  strcpy(iac->adrPachetNou, argv[cnt + 1]);
	} else if(strcmp(argv[cnt], "-m")) {
      /* ar trebui să urmeze mesajul de modificări asociat acestei versiuni în argv[cnt + 1] */
	  strcpy(iac->mesajModificari, argv[cnt + 1]);
	} else if(strcmp(argv[cnt], "-v")) {
      /* în argv[cnt + 1] ar trebui să găsim versiunea */
	  iac->vers = atof(argv[cnt + 1]);
	}
  }
  
  if(strlen(iac->adrPachetNou) == 0) {
    /* nu există nici măcar adresa pachetului printre argumentele de invocare.
     * Se poate considera că invocare este validă prin interpretarea de argumente. */
     free(iac->adrPachetNou);
     free(iac->mesajModificari);
     free(iac);
     iac = NULL;
  }
  
  return iac;
}

int main(int argc, char *argv[]) {
  gtk_init(&argc, &argv);
  
  IntrareActualizare *infoActualizareExtern = NULL;
  
  if(db_initializeaza()) {
	
	/* încărcăm eventualele informații venite din exterior (în parametrii dați prin linia de comandă) */
	if(argc != 1) {
	  infoActualizareExtern = reconstituieCerereDeActualizare(argc, argv);
	}
	
	if(db_obtine_este_prima_rulare() &&
	    infoActualizareExtern == NULL) {
	  g_debug("Afișez formularul de 'primă rulare' ...");
	  
	  GtkWidget *frmPrimaIntrebare = NULL;
	  
	  /* pare-se că este prima execuție a aplicației (de la instalare ?).
	   * Interoghează-l pe utilizator privind modul de actualizare dorit */
	  frmPrimaIntrebare = gtk_message_dialog_new_with_markup(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, 
                                                  GTK_BUTTONS_NONE, "Având în vedere că aceasta este prima dată când deschideți aplicația,\n\n"
                                                "<b>cum doriți să se realizeze actualizările ?</b>\n\n"
                                                " > <b>Răspund mai târziu</b>: mă mai gândesc,\n"
                                                " > <b>Manual</b>: prin butonul de pe formularul de informații, sau\n"
                                                " > <b>În mod automat</b>: la fiecare pornire a aplicației.");
	  gtk_window_set_title(GTK_WINDOW(frmPrimaIntrebare), "Întrebare");
	  gtk_window_set_position(GTK_WINDOW(frmPrimaIntrebare), GTK_WIN_POS_CENTER);
	  gtk_dialog_add_buttons(GTK_DIALOG(frmPrimaIntrebare), "Răspund mai târziu", RPS_PRIMA_INTREB_RASPUNS_MAI_TARZIU,
													        "Actualizez manual", RPS_PRIMA_INTREB_RASPUNS_MANUAL,
													        "Actualizează automat", RPS_PRIMA_INTREB_RASPUNS_AUTOMAT,
													        NULL);
	  gtk_dialog_set_default_response(GTK_DIALOG(frmPrimaIntrebare), RPS_PRIMA_INTREB_RASPUNS_AUTOMAT);
      gtk_widget_show_all(frmPrimaIntrebare);
      g_signal_connect(frmPrimaIntrebare, "response", G_CALLBACK(frmPrimaIntrebare_click_raspuns), NULL);
      
	  gtk_main();
	} else {
	  g_debug("Aceasta nu este prima execuție a aplicației ...");
	}
	
	if(db_obtine_este_actualizare_automata() ||
	   infoActualizareExtern != NULL) {
	  g_debug("Utilizatorul fie dorește actualizare automată ori de câte ori pornește aplicația fie actualizatorul a fost invocat de către programul principal.");
	  if(dl_initializeaza(db_obtine_adresa_actualizare())) {
		g_debug("Modulul de descărcări (downloads : 'dl') a fost inițializat cu succes.");
        if(infoActualizareExtern != NULL ||
           dl_exista_versiune_mai_buna_decat(db_obtine_versiune_curenta())) {
	      g_debug("S-a analizat și s-a găsit o versiune de aplicație mai nouă. Începe procedeul de actualizare ...");
	        
	      if(infoActualizareExtern != NULL) {
		    /* procesul curent a fost scurtcircuitat de parametrii valizi dați în linia de comandă */
		    dl_seteaza_ultima_intrare_actualizare(infoActualizareExtern);
          } else {
            /* execuția curentă a fost comandată direct de către utilizator */
            //infoActualizareExtern = dl_obtine_ultima_intrare_actualizare();
          }
	      
	      GtkWidget *dlgRezultatActualizare = NULL;
	      if(dl_actualizeaza_aplicatia()) {
            dlgRezultatActualizare = gtk_message_dialog_new_with_markup(NULL, GTK_DIALOG_MODAL,
                                                                        GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                                                        "Actualizarea s-a realizat cu <u>succes</u>.\n\n"
                                                                        "Versiunea curentă a programului este <b>%.2lf</b>.",
                                                                        db_obtine_versiune_curenta());
		    gtk_window_set_title(GTK_WINDOW(dlgRezultatActualizare), "Succes!");
		  } else {
            dlgRezultatActualizare = gtk_message_dialog_new_with_markup(NULL, GTK_DIALOG_MODAL,
                                                                        GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
                                                                        "A existat <u>o problemă</u> la actualizare!\n\n"
                                                                        "Vă rugăm încercați mai târziu, iar dacă problema persistă, atunci "
                                                                        "contactați unul din programatori.\n\n"
                                                                        "<b>Vă mulțumim pentru înțelegere și <u>ne pare rău</u> pentru cele întâmplate!</b>");
		    gtk_window_set_title(GTK_WINDOW(dlgRezultatActualizare), "Problemă!");
		  }
		  gtk_dialog_run(GTK_DIALOG(dlgRezultatActualizare));
		  gtk_widget_destroy(dlgRezultatActualizare);
	    } else {
          g_debug("Nu s-a găsit o versiune mai bună a aplicației.");
        }
	  }
	    
      dl_curata();
    }
    db_curata();
  } else {
    g_warning("Nu am putut inițializa modulul bazei de date. Procesul de actualizare nu a putut avea loc!");
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
