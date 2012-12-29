/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include <glib/gprintf.h>

#include "sda.h"
#include "fl.h"
#include "db.h"
#include "os.h"

#include "finfo.h"

/* ATENȚIE: a nu se muta această incluziune în altă parte pentru că G_OS_WIN32 se definește în gtk/gtk.h care este inclus în 'finfo.h' */
#if defined(G_OS_WIN32) && !defined(_WINDOWS_H)
#include <windows.h>
#endif

#define FINFO_ACTCONF_DLG_DA       0
#define FINFO_ACTCONF_DLG_NU       1
#define FINFO_ACTCONF_DLG_INREGULA 2

static void incarca_info_general(FInfoInstanta *fii, GtkWidget *cadruFrm);
static gboolean frmInfo_delev(GtkWidget *widget, GdkEvent *event, gpointer data);
static gboolean frmInfo_la_dezapasare_taste(GtkWidget *widget, GdkEventKey *ke, FInfoInstanta *fii);
static void btIesire_clicked(GtkWidget *widget, FInfoInstanta *fii);
static void btActualizeaza_clicked(GtkWidget *widget, FInfoInstanta *fii);
static GtkWidget *finfo_creeaza_buton(GtkWindow *fereastraParinte,
        const gchar *textAfisat, fl_media_type tpImg,
        const gchar *textIndicatie);

FInfoInstanta *finfo_initializeaza(GtkWindow *parinte) {
    FInfoInstanta *fii = NULL;
    GtkWidget *cadruFrm = NULL;
    GtkWidget *imgInfo = NULL;

    if ((fii = g_new0(FInfoInstanta, 1)) == NULL) {
        g_warning("Nu am putut aloca structura formularului de informații!");
    } else {
        /* inițializăm formularul de informații */
        fii->frm = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(fii->frm), "psAle ~ Despre aplicație ...");
        gtk_window_set_icon(GTK_WINDOW(fii->frm), fl_obtine_imagine_media_scalata(FL_GENERAL_IMG_FORMULARE, -1, -1));
        gtk_window_set_transient_for(GTK_WINDOW(fii->frm), parinte);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(fii->frm), TRUE);
        gtk_window_set_position(GTK_WINDOW(fii->frm), GTK_WIN_POS_CENTER_ON_PARENT);
        gtk_container_set_border_width(GTK_CONTAINER(fii->frm), 5);
        gtk_window_set_modal(GTK_WINDOW(fii->frm), TRUE);
        gtk_window_set_resizable(GTK_WINDOW(fii->frm), FALSE);
        g_signal_connect(fii->frm, "key-release-event", G_CALLBACK(frmInfo_la_dezapasare_taste), fii);
        g_signal_connect(fii->frm, "delete-event", G_CALLBACK(frmInfo_delev), NULL);

        /* inițializăm cadrul formularului */
        cadruFrm = gtk_table_new(4, 4, FALSE);
        gtk_table_set_row_spacings(GTK_TABLE(cadruFrm), 3);
        gtk_table_set_col_spacings(GTK_TABLE(cadruFrm), 2);
        gtk_container_add(GTK_CONTAINER(fii->frm), cadruFrm);

        incarca_info_general(fii, cadruFrm);

        /* inițializăm imaginea de informație */
        GdkPixbuf *pixInfoPictograma = fl_obtine_imagine_media_scalata(FL_IMG_FINFO_INFO, -1, -1);
        imgInfo = gtk_image_new_from_pixbuf(pixInfoPictograma);
        g_object_unref(pixInfoPictograma);
        gtk_table_attach_defaults(GTK_TABLE(cadruFrm), imgInfo, 0, 1, 0, 1);

        /* inițializăm butoanele formularului */
        fii->btActualizeaza = finfo_creeaza_buton(GTK_WINDOW(fii->frm),
                "Actualizează", FL_IMG_FINFO_ACTUALIZEAZA,
                "Verifică și aplică eventualele îmbunătățiri ale aplicației.\nTaste scurte: <i>Ctrl + R</i>");
        g_signal_connect(fii->btActualizeaza, "clicked", G_CALLBACK(btActualizeaza_clicked), fii);
        gtk_table_attach_defaults(GTK_TABLE(cadruFrm), fii->btActualizeaza, 2, 3, 3, 4);

        fii->btParasesteFrm = finfo_creeaza_buton(GTK_WINDOW(fii->frm),
                "Părăsește formular", FL_IMG_FINFO_PARASESTE,
                "Închide formularul de informații.\nTastă scurtă: <i>Esc</i>");
        g_signal_connect(fii->btParasesteFrm, "clicked", G_CALLBACK(btIesire_clicked), fii);
        gtk_table_attach_defaults(GTK_TABLE(cadruFrm), fii->btParasesteFrm, 3, 4, 3, 4);
    }

    return fii;
}

void finfo_curata(FInfoInstanta **fii) {
    if (fii == NULL) return;

    gtk_widget_destroy((*fii)->frm);
    g_free(fii);
}

void finfo_arata(FInfoInstanta *fii) {
    g_assert(fii != NULL);

    gtk_widget_show_all(fii->frm);
    //gtk_main();
}

static gboolean imgLicenta_click(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
#ifdef G_OS_WIN32
    ShellExecute(NULL, "open", "http://creativecommons.org/licenses/by-nc-sa/3.0/ro/", NULL, NULL, SW_SHOWNORMAL);
#elif defined G_OS_UNIX
    system("xdg-open 'http://creativecommons.org/licenses/by-nc-sa/3.0/ro/' &");
#endif

    return TRUE;
}

static gboolean frmInfo_delev(GtkWidget *widget, GdkEvent *event, gpointer data) {
    return FALSE;
}

static gboolean frmInfo_la_dezapasare_taste(GtkWidget *widget, GdkEventKey *ke, FInfoInstanta *fii) {
    gboolean evGestionat = FALSE;

    if (ke->keyval == GDK_KEY_Escape) {
        /* tasta 'Esc' a fost apăsată. Părăsește formularul. */
        btIesire_clicked(widget, fii);
        evGestionat = TRUE;
    } else if ((ke->state & GDK_CONTROL_MASK) != 0) {
        switch (ke->keyval) {
            case GDK_KEY_R:
            case GDK_KEY_r:
                /* Ctrl + R apăsat. Declanșează secvența de actualizare. */
                btActualizeaza_clicked(widget, fii);
                break;
        }
    }

    return evGestionat;
}

static void btIesire_clicked(GtkWidget *widget, FInfoInstanta *fii) {
    finfo_curata(&fii);
}

static void btActualizeaza_clicked(GtkWidget *widget, FInfoInstanta *fii) {
    gboolean actualizareConfirmata = FALSE;
    GtkWidget *dlgIntrebareActualizare = NULL;
    Versiune *versServer = NULL;
    Versiune *versLocala = NULL;
    GtkWidget *dlgEsecInActualziare = NULL;

    if ((versServer = os_rpsale_obtine_versiune_server()) == NULL) {
        g_warning("Nu am putut obține starea versiunilor de pe server astfel încât actualizarea nu s-a putut realiza!");
        dlgEsecInActualziare = gtk_message_dialog_new_with_markup(GTK_WINDOW(fii->frm), GTK_DIALOG_MODAL,
                GTK_MESSAGE_WARNING, GTK_BUTTONS_NONE,
                "Am întâmpinat dificultăți în etapa de obținere a versiunii curente de pe server!\n"
                "Vă rugăm încercați din nou un pic mai târziu ...");

        gtk_window_set_title(GTK_WINDOW(dlgEsecInActualziare), "Ne-realizat");
        gtk_dialog_add_buttons(GTK_DIALOG(dlgEsecInActualziare), "În regulă", 0, NULL);
        gtk_dialog_set_default_response(GTK_DIALOG(dlgEsecInActualziare), 0);
        gtk_dialog_run(GTK_DIALOG(dlgEsecInActualziare));
        gtk_widget_destroy(dlgEsecInActualziare);
        return;
    } else if ((versLocala = db_obtine_versiune_curenta()) == NULL) {
        g_warning("Nu am putut determina versiunea curentă a pachetului SW, iar pentru asta am oprit secvența de actualizare!");
        return;
    }

    if (sda_comparaVersiuni(versServer, versLocala) == 1) {
        g_debug("S-a analizat și s-a găsit o versiune de aplicație mai nouă. Întreabă utilizatorul privind acțiunea următoare ...");

        dlgIntrebareActualizare = gtk_message_dialog_new_with_markup(GTK_WINDOW(fii->frm), GTK_DIALOG_MODAL,
                GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE,
                "Am găsit o actualizare.\n"
                "Se pare că ultima versiune este <b>%s</b> !\n\n"
                "Doriți să treceți <i>acum</i> la această nouă versiune ?\n\n"
                "<b>Atenție!</b> Dacă răspundeți <b><i>Da</i></b> atunci aplicația curentă <u>se va închide</u>! "
                "Asigurați-vă că nu pierdeți nimic din lucrul curent.",
                sda_versiune_printf(versServer));

        gtk_window_set_title(GTK_WINDOW(dlgIntrebareActualizare), "Întrebare");
        gtk_dialog_add_buttons(GTK_DIALOG(dlgIntrebareActualizare), "Da", FINFO_ACTCONF_DLG_DA,
                "Nu", FINFO_ACTCONF_DLG_NU,
                NULL);
        gtk_dialog_set_default_response(GTK_DIALOG(dlgIntrebareActualizare), FINFO_ACTCONF_DLG_DA);
        if (gtk_dialog_run(GTK_DIALOG(dlgIntrebareActualizare)) == FINFO_ACTCONF_DLG_DA) {
            actualizareConfirmata = TRUE;
        }
    } else {
        g_debug("Nu am găsit nici o actualizare sau s-a întâmplat ceva cu procesul de actualizare ... ");

        dlgIntrebareActualizare = gtk_message_dialog_new_with_markup(GTK_WINDOW(fii->frm), GTK_DIALOG_MODAL,
                GTK_MESSAGE_INFO, GTK_BUTTONS_NONE,
                "Nu am găsit nicio actualizare disponibilă.\n\n"
                "Versiunea pe care o aveți, <b>%s</b>, este ultima.",
                sda_versiune_printf(versLocala));

        gtk_window_set_title(GTK_WINDOW(dlgIntrebareActualizare), "Rezultat");
        gtk_dialog_add_buttons(GTK_DIALOG(dlgIntrebareActualizare), "Am înțeles", FINFO_ACTCONF_DLG_INREGULA,
                NULL);
        gtk_dialog_set_default_response(GTK_DIALOG(dlgIntrebareActualizare), FINFO_ACTCONF_DLG_INREGULA);
        gtk_dialog_run(GTK_DIALOG(dlgIntrebareActualizare));
    }

    gtk_widget_destroy(dlgIntrebareActualizare);

    if (TRUE == actualizareConfirmata) {
        /* avem actualizare și acordul utilizatorului de a o aplica. Îi dăm drumul lui 'rpsAle' să treacă la treabă! */
        os_rpsale_forteaza_actualizare(versServer);
    }
    g_free(versLocala);
    g_free(versServer);

    if (TRUE == actualizareConfirmata) {
        /* totul este pregătit pentru actualizare. 'rpsAle' e pornit, iar nouă nu ne mai rămâne decât să închidem 'psAle'
         * și să lăsăm actualizatorul să-și facă treaba */
        gtk_main_quit();
    }
}

static void incarca_info_general(FInfoInstanta *fii, GtkWidget *cadruFrm) {
    GtkWidget *cadruTitluEtichete = NULL;
    GtkWidget *cadruValEtichete = NULL;
    gchar versActualaLocala[10];
    Versiune *versCurenta = NULL;

    if ((versCurenta = db_obtine_versiune_curenta()) != NULL) {
        g_sprintf(versActualaLocala, "%s", sda_versiune_printf(versCurenta));
        g_free(versCurenta);
    } else {
        g_debug("Nu am putut obține versiunea curentă din baza de date pentru a o afișa în formularul de informații!");
        g_sprintf(versActualaLocala, "er_bd");
    }
    /* inițializăm cadrul etichetelor (titlu + valoare) */
    cadruTitluEtichete = gtk_vbox_new(FALSE, 4);
    gtk_table_attach_defaults(GTK_TABLE(cadruFrm), cadruTitluEtichete, 1, 2, 0, 1);

    GtkWidget *lblTitluAutor = gtk_label_new("Autor : ");
    GtkWidget *lblTitluVersiune = gtk_label_new("Versiune : ");
    GtkWidget *lblTitluLicenta = gtk_label_new("Licență : ");

    gtk_misc_set_alignment(GTK_MISC(lblTitluAutor), 1.0f, 0.1f);
    gtk_misc_set_alignment(GTK_MISC(lblTitluVersiune), 1.0f, 0.1f);
    gtk_misc_set_alignment(GTK_MISC(lblTitluLicenta), 1.0f, 0.1f);

    gtk_container_add(GTK_CONTAINER(cadruTitluEtichete), lblTitluAutor);
    gtk_container_add(GTK_CONTAINER(cadruTitluEtichete), lblTitluVersiune);
    gtk_container_add(GTK_CONTAINER(cadruTitluEtichete), lblTitluLicenta);

    cadruValEtichete = gtk_vbox_new(FALSE, 4);
    gtk_table_attach_defaults(GTK_TABLE(cadruFrm), cadruValEtichete, 2, 3, 0, 1);

    GtkWidget *lblValAutor = gtk_label_new(PSALE_NUME_AUTOR);
    GtkWidget *lblValVersiune = gtk_label_new(versActualaLocala);
    GdkPixbuf *pixImgLicenta = fl_obtine_imagine_media_scalata(FL_IMG_FINFO_LICENTA, -1, -1);
    GtkWidget *imgLicenta = gtk_image_new_from_pixbuf(pixImgLicenta);
    g_object_unref(pixImgLicenta);
    GtkWidget *cadruImgLicenta = gtk_event_box_new();

    gtk_misc_set_alignment(GTK_MISC(lblValAutor), 1.0f, 0.1f);
    gtk_misc_set_alignment(GTK_MISC(lblValVersiune), 1.0f, 0.1f);
    gtk_misc_set_alignment(GTK_MISC(imgLicenta), 1.0f, 0.1f);
    gtk_widget_set_tooltip_markup(cadruImgLicenta, "Prezintă informații sumare despre licența aplicației.\n<i>(click pentru a o deschide online)</i>");
    g_signal_connect(cadruImgLicenta, "button-press-event", G_CALLBACK(imgLicenta_click), NULL);
    gtk_container_add(GTK_CONTAINER(cadruImgLicenta), imgLicenta);

    gtk_container_add(GTK_CONTAINER(cadruValEtichete), lblValAutor);
    gtk_container_add(GTK_CONTAINER(cadruValEtichete), lblValVersiune);
    gtk_container_add(GTK_CONTAINER(cadruValEtichete), cadruImgLicenta);

    /* inițializăm regiunea de informații generale (și scurtături ?) */
    GtkWidget *lblDespre = gtk_label_new("Despre : ");

    gtk_misc_set_alignment(GTK_MISC(lblDespre), 1.0f, 0.02f);
    gtk_table_attach_defaults(GTK_TABLE(cadruFrm), lblDespre, 1, 2, 1, 2);

    fii->txtInfoVersiuni = gtk_text_view_new();
    GtkTextBuffer *txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fii->txtInfoVersiuni));

    gtk_text_buffer_set_text(txtBuffer, PSALE_TEXT_DESPRE, -1);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(fii->txtInfoVersiuni), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(fii->txtInfoVersiuni), GTK_WRAP_WORD);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(fii->txtInfoVersiuni), FALSE);
    gtk_table_attach_defaults(GTK_TABLE(cadruFrm), fii->txtInfoVersiuni, 2, 4, 1, 2);
}

static GtkWidget *finfo_creeaza_buton(GtkWindow *fereastraParinte,
        const gchar *textAfisat, fl_media_type tpImg,
        const gchar *textIndicatie) {
    GtkWidget *btRezultat = NULL;

    /* inițializări generale */
    btRezultat = gtk_button_new_with_label(textAfisat);
    gtk_button_set_relief(GTK_BUTTON(btRezultat), GTK_RELIEF_HALF);
    gtk_button_set_focus_on_click(GTK_BUTTON(btRezultat), FALSE);
    gtk_widget_set_tooltip_markup(btRezultat, textIndicatie);

    /* atașăm pictograma */
    GdkPixbuf *imgbtPixBuf = fl_obtine_imagine_media_scalata(tpImg, 16, 16);
    GtkWidget *imgbt = gtk_image_new_from_pixbuf(imgbtPixBuf);
    g_object_unref(imgbtPixBuf);
    gtk_button_set_image_position(GTK_BUTTON(btRezultat), GTK_POS_RIGHT);
    gtk_button_set_image(GTK_BUTTON(btRezultat), imgbt);

    return btRezultat;
}
