/*
 * rpsAle - psAle's updater application
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include "fprimarulare.h"

static void fpr_click_raspuns(GtkDialog *dialog, gint idRaspuns, FPRInstanta *fpri);

FPRInstanta *fpr_instantiaza() {
    FPRInstanta *deRaspuns = g_new0(FPRInstanta, 1);

    deRaspuns->frm = gtk_message_dialog_new_with_markup(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION,
            GTK_BUTTONS_NONE, "Având în vedere că aceasta este prima dată când deschideți aplicația,\n\n"
            "<b>cum doriți să se realizeze actualizările ?</b>\n\n"
            " > <b>Răspund mai târziu</b>: mă mai gândesc,\n"
            " > <b>Manual</b>: prin butonul de pe formularul de informații, sau\n"
            " > <b>În mod automat</b>: la fiecare pornire a aplicației.");
    gtk_window_set_title(GTK_WINDOW(deRaspuns->frm), "Întrebare");
    gtk_window_set_position(GTK_WINDOW(deRaspuns->frm), GTK_WIN_POS_CENTER);
    gtk_dialog_add_buttons(GTK_DIALOG(deRaspuns->frm), "Răspund mai târziu", FPR_RASPUNS_MAI_TARZIU,
            "Actualizez manual", FPR_RASPUNS_MANUAL,
            "Actualizează automat", FPR_RASPUNS_AUTOMAT,
            NULL);
    gtk_dialog_set_default_response(GTK_DIALOG(deRaspuns->frm), FPR_RASPUNS_AUTOMAT);
    gtk_widget_show_all(deRaspuns->frm);
    g_signal_connect(deRaspuns->frm, "response", G_CALLBACK(fpr_click_raspuns), deRaspuns);

    return deRaspuns;
}

void fpr_elibereaza(FPRInstanta **fpri) {
    g_assert(fpri != NULL);

    if((*fpri)->frm != NULL) gtk_widget_destroy((*fpri)->frm);
    g_free(*fpri);
}

enum FPRRaspunsActualizare fpr_lanseaza_si_asteapta_raspuns(FPRInstanta *fpri) {
    g_assert(fpri != NULL);
    
    gtk_main();
    return fpri->tipActDorit;
}

static void fpr_click_raspuns(GtkDialog *dialog, gint idRaspuns, FPRInstanta *fpri) {
    switch(idRaspuns) {
        case GTK_RESPONSE_DELETE_EVENT:
            fpri->tipActDorit = FPR_RASPUNS_MAI_TARZIU;
            break;
        default:
            fpri->tipActDorit = idRaspuns;
            break;
    }
    
    gtk_main_quit();
}