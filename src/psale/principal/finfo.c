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
#include <gtk-2.0/gdk/gdk.h>
#include <gtk-2.0/gtk/gtklabel.h>
#include <gtk-2.0/gtk/gtkwidget.h>

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
static void frmInfo_seteaza_stare_actualizare(FInfoInstanta *fii, FIStareActualizare stareNoua, gchar *argvPrint);
static void frmInfo_seteaza_stare_actualizare_fp(FInfoInstanta *fii, FIStareActualizare stareNoua, gchar *argvPrint);
static gboolean frmInfo_la_click_uri_lblActualizare(GtkLabel *lbl, gchar *uri, FInfoInstanta *fii);
static gboolean frmInfo_cauta_actualizari(FInfoInstanta *fii);
static GtkWidget *finfo_creeaza_buton(GtkWindow *fereastraParinte,
        const gchar *textAfisat, FLImgTipMedia tpImg,
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
        gtk_table_attach_defaults(GTK_TABLE(cadruFrm), imgInfo, 0, 2, 0, 1);

        /* inițializăm butoanele formularului */
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
    g_free(*fii);
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
                frmInfo_la_click_uri_lblActualizare(GTK_LABEL(fii->lblStareActualizare), FI_ACTUALIZARE_URI_PORNESTE, fii);
                break;
        }
    }

    return evGestionat;
}

static void btIesire_clicked(GtkWidget *widget, FInfoInstanta *fii) {
    finfo_curata(&fii);
}

static void incarca_info_general(FInfoInstanta *fii, GtkWidget *cadruFrm) {
    GtkWidget *cadruEtichete = NULL;
    GtkWidget *cadruEtichAutor = NULL;
    GtkWidget *cadruEtichVersiune = NULL;
    GtkWidget *cadruEtichLicenta = NULL;
    GtkWidget *lblTitluAutor = NULL;
    GtkWidget *lblValAutor = NULL;
    GtkWidget *lblTitluVersiune = NULL;
    GtkWidget *lblValVersiune = NULL;
    GtkWidget *lblTitluLicenta = NULL;
    GdkPixbuf *pixImgLicenta = NULL;
    GtkWidget *imgLicenta = NULL;
    GtkWidget *cadruImgLicenta = NULL;
    gchar sirVersActualaLocala[32];
    Versiune *versCurenta = NULL;
            
    if ((versCurenta = db_obtine_versiune_curenta()) != NULL) {
        g_sprintf(sirVersActualaLocala, "<b>%s</b> ,", sda_versiune_printf(versCurenta));
        g_free(versCurenta);
    } else {
        g_debug("Nu am putut obține versiunea curentă din baza de date pentru a o afișa în formularul de informații!");
        g_sprintf(sirVersActualaLocala, "<b>er_bd</b> ,");
    }
    /* inițializăm cadrul etichetelor (titlu + valoare) */
    cadruEtichete = gtk_vbox_new(FALSE, 10);
    gtk_table_attach_defaults(GTK_TABLE(cadruFrm), cadruEtichete, 2, 4, 0, 1);

    /* construim și adăugăm informații despre autor */
    cadruEtichAutor = gtk_hbox_new(FALSE, 0);
    lblTitluAutor = gtk_label_new("Autor : ");
    lblValAutor = gtk_label_new(PSALE_NUME_AUTOR);
    
    gtk_widget_set_size_request(lblTitluAutor, 100, -1);
    gtk_misc_set_alignment(GTK_MISC(lblTitluAutor), 1, 0);
    gtk_box_pack_start(GTK_BOX(cadruEtichAutor), lblTitluAutor, FALSE, FALSE, 0);
    gtk_misc_set_alignment(GTK_MISC(lblValAutor), 1, 0);
    gtk_box_pack_start(GTK_BOX(cadruEtichAutor), lblValAutor, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(cadruEtichete), cadruEtichAutor, TRUE, TRUE, 0);
    
    /* construim și adăugăm informații despre versiune */
    cadruEtichVersiune = gtk_hbox_new(FALSE, 0);
    lblTitluVersiune = gtk_label_new("Versiune : ");
    lblValVersiune = gtk_label_new(NULL);
 
    gtk_widget_set_size_request(lblTitluVersiune, 100, -1);
    gtk_misc_set_alignment(GTK_MISC(lblTitluVersiune), 1, 0);
    gtk_box_pack_start(GTK_BOX(cadruEtichVersiune), lblTitluVersiune, FALSE, FALSE, 0);
    gtk_label_set_markup(GTK_LABEL(lblValVersiune), sirVersActualaLocala);
    gtk_misc_set_alignment(GTK_MISC(lblValVersiune), 1, 0);
    gtk_box_pack_start(GTK_BOX(cadruEtichVersiune), lblValVersiune, FALSE, FALSE, 0);
    fii->lblStareActualizare = gtk_label_new(NULL);
    gtk_misc_set_alignment(GTK_MISC(fii->lblStareActualizare), 1, 0);
    gtk_box_pack_start(GTK_BOX(cadruEtichVersiune), fii->lblStareActualizare, FALSE, FALSE, 7);
    fii->imgStareActualizare = gtk_image_new();
    gtk_misc_set_alignment(GTK_MISC(fii->imgStareActualizare), 1, 0);
    gtk_box_pack_start(GTK_BOX(cadruEtichVersiune), fii->imgStareActualizare, FALSE, FALSE, 0);
    frmInfo_seteaza_stare_actualizare(fii, FI_ACTUALIZARE_INITIALIZARE, NULL);
    
    g_signal_connect(fii->lblStareActualizare, "activate-link", G_CALLBACK(frmInfo_la_click_uri_lblActualizare), fii);
    gtk_box_pack_start(GTK_BOX(cadruEtichete), cadruEtichVersiune, TRUE, TRUE, 0);
    
    /* construim și adăugăm informații despre licență */
    cadruEtichLicenta = gtk_hbox_new(FALSE, 0);
    lblTitluLicenta = gtk_label_new("Licență : ");
    pixImgLicenta = fl_obtine_imagine_media_scalata(FL_IMG_FINFO_LICENTA, -1, -1);
    imgLicenta = gtk_image_new_from_pixbuf(pixImgLicenta);
    g_object_unref(pixImgLicenta);
    cadruImgLicenta = gtk_event_box_new();
    
    gtk_widget_set_size_request(lblTitluLicenta, 100, -1);
    gtk_misc_set_alignment(GTK_MISC(lblTitluLicenta), 1, 0);
    gtk_misc_set_alignment(GTK_MISC(imgLicenta), 1, 0);
    gtk_widget_set_tooltip_markup(cadruImgLicenta, "Prezintă informații sumare despre licența aplicației.\n<i>(click pentru a o deschide online)</i>");
    g_signal_connect(cadruImgLicenta, "button-press-event", G_CALLBACK(imgLicenta_click), NULL);
    gtk_container_add(GTK_CONTAINER(cadruImgLicenta), imgLicenta);
    
    gtk_box_pack_start(GTK_BOX(cadruEtichLicenta), lblTitluLicenta, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(cadruEtichLicenta), cadruImgLicenta, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(cadruEtichete), cadruEtichLicenta, TRUE, TRUE, 0);

    /* inițializăm regiunea de informații generale (și scurtături ?) */
    GtkWidget *lblDespre = gtk_label_new("Despre : ");

    gtk_misc_set_alignment(GTK_MISC(lblDespre), 1.0f, 0.02f);
    gtk_table_attach_defaults(GTK_TABLE(cadruFrm), lblDespre, 0, 1, 1, 2);

    fii->txtInfoVersiuni = gtk_text_view_new();
    GtkTextBuffer *txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fii->txtInfoVersiuni));

    gtk_text_buffer_set_text(txtBuffer, PSALE_TEXT_DESPRE, -1);
    gtk_widget_set_size_request(fii->txtInfoVersiuni, 450, 200);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(fii->txtInfoVersiuni), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(fii->txtInfoVersiuni), GTK_WRAP_WORD);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(fii->txtInfoVersiuni), FALSE);
    gtk_table_attach_defaults(GTK_TABLE(cadruFrm), fii->txtInfoVersiuni, 1, 4, 1, 2);
}

static void frmInfo_seteaza_stare_actualizare(FInfoInstanta *fii, FIStareActualizare stareNoua, gchar *argvPrint) {
    g_assert(fii != NULL);

    GdkPixbuf *pxBufSimplu = NULL;
    GdkPixbufAnimation *pxBufAnimatie = NULL;
    gchar mesajLblStare[128];
    
    switch (stareNoua) {
        case FI_ACTUALIZARE_INITIALIZARE:
            pxBufSimplu = fl_obtine_imagine_media_scalata(FL_IMG_FINFO_ACTUALIZARE_REPAUS, FI_IMG_STARE_ACTUALIZARE_LUNGIME, FI_IMG_STARE_ACTUALIZARE_INALTIME);
            gtk_image_set_from_pixbuf(GTK_IMAGE(fii->imgStareActualizare), pxBufSimplu);
            g_sprintf(mesajLblStare, "<a href='"FI_ACTUALIZARE_URI_PORNESTE"'>verificați</a>");
            break;
        case FI_ACTUALIZARE_IN_CURS:
            pxBufAnimatie = fl_obtine_animatie_media(FL_ANIM_FINFO_ACTUALIZARE_PROGRES);
            gtk_image_set_from_animation(GTK_IMAGE(fii->imgStareActualizare), pxBufAnimatie);
            g_sprintf(mesajLblStare, "caut");
            break;
        case FI_ACTUALIZARE_SUCCES_VERSNOUA:
            pxBufSimplu = fl_obtine_imagine_media_scalata(FL_IMG_FINFO_ACTUALIZARE_SUCCES_VNOUA, FI_IMG_STARE_ACTUALIZARE_LUNGIME, FI_IMG_STARE_ACTUALIZARE_INALTIME);
            gtk_image_set_from_pixbuf(GTK_IMAGE(fii->imgStareActualizare), pxBufSimplu);
            g_sprintf(mesajLblStare, "disponbilă: '<b><u>%s</u></b>'. <a href='"FI_ACTUALIZARE_URI_ACTUALIZEAZA":%s'>Actualizați</a>.",
                    (argvPrint == NULL ? "0.0" : argvPrint),
                    (argvPrint == NULL ? "0.0" : argvPrint));
            break;
        case FI_ACTUALIZARE_SUCCES_VERSNESCHIMBATA:
            pxBufSimplu = fl_obtine_imagine_media_scalata(FL_IMG_FINFO_ACTUALIZARE_SUCCES_VNENOUA, FI_IMG_STARE_ACTUALIZARE_LUNGIME, FI_IMG_STARE_ACTUALIZARE_INALTIME);
            gtk_image_set_from_pixbuf(GTK_IMAGE(fii->imgStareActualizare), pxBufSimplu);
            g_sprintf(mesajLblStare, "este ultima");
            break;
        case FI_ACTUALIZARE_ESEC:
            pxBufSimplu = fl_obtine_imagine_media_scalata(FL_IMG_FINFO_ACTUALIZARE_ESUATA, FI_IMG_STARE_ACTUALIZARE_LUNGIME, FI_IMG_STARE_ACTUALIZARE_INALTIME);
            gtk_image_set_from_pixbuf(GTK_IMAGE(fii->imgStareActualizare), pxBufSimplu);
            g_sprintf(mesajLblStare, "Problemă (%s), încercați mai târziu.", (argvPrint == NULL ? "-1" : argvPrint));
            break;
        default:
            g_warning("Nu recunosc starea actualizării furnizate!");
            break;
    }
    gtk_label_set_markup(GTK_LABEL(fii->lblStareActualizare), mesajLblStare);
    if (pxBufSimplu != NULL) g_object_unref(pxBufSimplu);
    if (pxBufAnimatie != NULL) g_object_unref(pxBufAnimatie);
}

static void frmInfo_seteaza_stare_actualizare_fp(FInfoInstanta *fii, FIStareActualizare stareNoua, gchar *argvPrint) {
    /* fp = 'fir protejat', util atunci când se dorește schimbarea stării din alte fire de execuție != de cel principal */
    gdk_threads_enter();
    frmInfo_seteaza_stare_actualizare(fii, stareNoua, argvPrint);
    gdk_threads_leave();
}

static gboolean frmInfo_la_click_uri_lblActualizare(GtkLabel *lbl, gchar *uri, FInfoInstanta *fii) {
    if (g_strcmp0(uri, FI_ACTUALIZARE_URI_PORNESTE) == 0) {
        GThread *firCautatorDeActualizari = NULL;
        GError *err = NULL;
        
        if((firCautatorDeActualizari = g_thread_try_new("cautActualizari", (GThreadFunc)(&frmInfo_cauta_actualizari), fii, &err)) == NULL) {
            g_warning("Nu am putut creea firul căutător de actualizări!Motiv : %s", err->message);
            g_error_free(err);
        }
    } else if (g_str_has_prefix(uri, FI_ACTUALIZARE_URI_ACTUALIZEAZA)) {
        GtkWidget *dlgIntrebareActualizare = NULL;

        dlgIntrebareActualizare = gtk_message_dialog_new_with_markup(GTK_WINDOW(fii->frm), GTK_DIALOG_MODAL,
                GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE,
                "Această operațiune de <b>actualizare directă</b> presupune închiderea aplicației curente.\n"
                "Înainte de a aplica această actualizare, asigurați-vă că toate sursele dumneavoastră sunt salvate pentru a nu pierde din lucru.\n\n"
                "<b>În aceste condiții, pot să încep actualizarea ?</b>");

        gtk_window_set_title(GTK_WINDOW(dlgIntrebareActualizare), "Întrebare");
        gtk_dialog_add_buttons(GTK_DIALOG(dlgIntrebareActualizare), "Da", FINFO_ACTCONF_DLG_DA,
                "Nu", FINFO_ACTCONF_DLG_NU,
                NULL);
        gtk_dialog_set_default_response(GTK_DIALOG(dlgIntrebareActualizare), FINFO_ACTCONF_DLG_DA);
        if (gtk_dialog_run(GTK_DIALOG(dlgIntrebareActualizare)) == FINFO_ACTCONF_DLG_DA) {
            gchar **elemInUri = g_strsplit(uri, ":", 2);
            Versiune *versTinta = sda_obtineVersiuneDinSir(elemInUri[1]);

            if (versTinta != NULL) {
                os_rpsale_forteaza_actualizare(versTinta);
            } else {
                g_warning("Nu am putut reconstitui versiunea țintă pentru actualizarea directă cerută!");
            }
            
            g_strfreev(elemInUri);
            g_free(versTinta);
            
            if(versTinta != NULL) {
                gtk_main_quit();
            }
        }
    }

    return TRUE;
}

static gboolean frmInfo_cauta_actualizari(FInfoInstanta *fii) {
    gboolean deReturnat = TRUE;
    Versiune *versServer = NULL;
    Versiune *versLocala = NULL;

    frmInfo_seteaza_stare_actualizare_fp(fii, FI_ACTUALIZARE_IN_CURS, NULL);
    if ((versServer = os_rpsale_obtine_versiune_server()) == NULL) {
        g_warning("Nu am putut obține starea versiunilor de pe server astfel încât actualizarea nu s-a putut realiza!");
        frmInfo_seteaza_stare_actualizare_fp(fii, FI_ACTUALIZARE_ESEC, "verServ");
    } else if ((versLocala = db_obtine_versiune_curenta()) == NULL) {
        g_warning("Nu am putut determina versiunea curentă a pachetului SW, iar pentru asta am oprit secvența de actualizare!");
        frmInfo_seteaza_stare_actualizare_fp(fii, FI_ACTUALIZARE_ESEC, "verLocal");
    } else {
        if (sda_comparaVersiuni(versServer, versLocala) == 1) {
            g_debug("S-a analizat și s-a găsit o versiune de aplicație mai nouă. Întreabă utilizatorul privind acțiunea următoare ...");
            frmInfo_seteaza_stare_actualizare_fp(fii, FI_ACTUALIZARE_SUCCES_VERSNOUA, sda_versiune_printf(versServer));
        } else {
            g_debug("Nu am găsit nicio actualizare disponibilă ... ");
            frmInfo_seteaza_stare_actualizare_fp(fii, FI_ACTUALIZARE_SUCCES_VERSNESCHIMBATA, NULL);
        }

        g_free(versLocala);
        g_free(versServer);
    }
    
    return deReturnat;
}

static GtkWidget *finfo_creeaza_buton(GtkWindow *fereastraParinte,
        const gchar *textAfisat, FLImgTipMedia tpImg,
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
