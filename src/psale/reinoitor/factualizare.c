/*
 * rpsAle - psAle's updater application
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include "factualizare.h"
#include "db.h"
#include "dz.h"

#include <glib/gprintf.h>

static void fa_progres_seteaza_stare(FAInstanta *fai, StareActualizare stareNoua);
static void fa_progres_seteaza_parametrii(FAInstanta *fai, gchar *txt);
static void fa_progres_actualizeaza_normal(FAInstanta *fai, gchar *txt);
static void fa_progres_actualizeaza_atentie(FAInstanta *fai, gchar *txt);
static gboolean fa_proceseaza_sesiune(FAInstanta *fai);
static gboolean fa_progres_animator(FAInstanta *fai);
static void fa_btAnuleaza_clicked(GtkButton *bt, FAInstanta *fai);
static gboolean fa_frm_delete_event(GtkWidget *widget, GdkEvent *ev, FAInstanta *fai);
static void fa_reseteaza_procesarea_sesiunii(FAInstanta *fai);

FAInstanta *fa_initializeaza(ParametriiRulareAplicatie *pari) {
    if (pari == NULL) return NULL;

    FAInstanta *strucSesiuneAct = NULL;
    GtkWidget *organizatorFormular = NULL;
    GtkWidget *containerBaraProgres = NULL;

    if ((strucSesiuneAct = g_new0(FAInstanta, 1)) != NULL) {
        strucSesiuneAct->masinaActualizanta = g_new0(AutomatonActualizare, 1);
        strucSesiuneAct->parametriInvocare = pari;
        strucSesiuneAct->esteBtAnuleazaApasat = FALSE;

        strucSesiuneAct->frm = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(strucSesiuneAct->frm), "'[r]eînoitor' ~ psAle");
        gtk_window_set_position(GTK_WINDOW(strucSesiuneAct->frm), GTK_WIN_POS_CENTER);
        gtk_widget_set_size_request(strucSesiuneAct->frm, 470, -1);
        gtk_container_border_width(GTK_CONTAINER(strucSesiuneAct->frm), 8);
        gtk_window_set_resizable(GTK_WINDOW(strucSesiuneAct->frm), FALSE);

        organizatorFormular = gtk_vbox_new(TRUE, 3);
        gtk_container_add(GTK_CONTAINER(strucSesiuneAct->frm), organizatorFormular);

        strucSesiuneAct->baraProgres = gtk_progress_bar_new();
        gtk_progress_bar_set_pulse_step(GTK_PROGRESS_BAR(strucSesiuneAct->baraProgres), 0.02);

        strucSesiuneAct->btAnuleaza = gtk_button_new();
        GtkWidget *imgBtAnuleaza = gtk_image_new_from_stock(GTK_STOCK_CANCEL, GTK_ICON_SIZE_BUTTON);
        gtk_button_set_image(GTK_BUTTON(strucSesiuneAct->btAnuleaza), imgBtAnuleaza);
        gtk_widget_set_tooltip_markup(strucSesiuneAct->btAnuleaza, "Anulează");
        gtk_widget_set_size_request(strucSesiuneAct->btAnuleaza, 40, -1);

        containerBaraProgres = gtk_hbox_new(FALSE, 2);
        gtk_box_pack_start(GTK_BOX(containerBaraProgres), strucSesiuneAct->baraProgres, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(containerBaraProgres), strucSesiuneAct->btAnuleaza, FALSE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(organizatorFormular), containerBaraProgres, TRUE, TRUE, 0);

        if (pari->folsesteIG) {
            gtk_widget_show_all(strucSesiuneAct->frm);
        }

        gtk_signal_connect(GTK_OBJECT(strucSesiuneAct->frm), "destroy", GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
        gtk_signal_connect(GTK_OBJECT(strucSesiuneAct->frm), "delete-event", G_CALLBACK(fa_frm_delete_event), strucSesiuneAct);
        gtk_signal_connect(GTK_OBJECT(strucSesiuneAct->btAnuleaza), "clicked", G_CALLBACK(fa_btAnuleaza_clicked), strucSesiuneAct);

        fa_progres_seteaza_stare(strucSesiuneAct, FA_DL_INITIALIZARE);
    } else {
        g_warning("Nu am putut inițializa structura principală a sesiunii de actualizare!");
    }

    return strucSesiuneAct;
}

void fa_curata(FAInstanta **fai) {
    if (fai == NULL) return;

    dl_curata(&(*fai)->masinaActualizanta->date);
    g_free((*fai)->masinaActualizanta);
    g_free(*fai);
    (*fai) = NULL;
}

void fa_lanseaza_sesiune_actualizare(FAInstanta *fai) {
    g_assert(fai != NULL);

    g_timeout_add(FA_MS_INTRE_STARI, (GSourceFunc) (&fa_proceseaza_sesiune), fai);
    g_timeout_add(20, (GSourceFunc) (&fa_progres_animator), fai);
    gtk_main();
}

static void fa_progres_seteaza_stare(FAInstanta *fai, StareActualizare stareNoua) {
    g_assert(fai != NULL);

    switch (stareNoua) {
        case FA_DL_INITIALIZARE:
            fa_progres_actualizeaza_normal(fai, "inițializez actualizatorul");
            break;
        case FA_DL_DESC_LISTA_ACTUALIZARI:
            fa_progres_actualizeaza_normal(fai, "aduc lista de actualizări");
            break;
        case FA_DL_CANALIZEAZA_ACTIUNE:
            fa_progres_actualizeaza_normal(fai, "filtrez acțiunea invocată");
            break;
        case FA_DL_PREGATESTE_VERSIUNE_TINTA:
            fa_progres_actualizeaza_normal(fai, "pregătesc versiunea țintă");
            break;
        case FA_DL_VERIFICA_VERSIUNE:
            fa_progres_actualizeaza_normal(fai, "verific necesitatea unei actualizări");
            break;
        case FA_DL_DESC_ACTUALIZARE:
            fa_progres_actualizeaza_normal(fai, "descarc actualizarea");
            break;
        case FA_DL_APLICARE_ACTUALIZARE:
            gtk_widget_set_sensitive(fai->btAnuleaza, FALSE);
            fa_progres_actualizeaza_normal(fai, "integrez actualizarea");
            break;

        case FA_DL_ATENTIE_MODUL_DL_NEINITIALIZAT:
            fa_progres_actualizeaza_atentie(fai, "modulul 'dl' NEinițializat");
            break;
        case FA_DL_ATENTIE_LISTA_ACTUALIZARI_NEDESCARCATA:
            fa_progres_actualizeaza_atentie(fai, "lista de actualizări NU este disponibilă");
            break;
        case FA_DL_ATENTIE_LISTA_ACTUALIZARI_GOALA:
            fa_progres_actualizeaza_atentie(fai, "lista de actualizări este GOALĂ");
            break;
        case FA_DL_ATENTIE_NU_RECUNOSC_ACTIUNEA:
            fa_progres_actualizeaza_atentie(fai, "NU RECUNOSC acțiunea cerută");
            break;
        case FA_DL_ATENTIE_NU_GASESC_VERSIUNEA_TINTA:
            fa_progres_actualizeaza_atentie(fai, "NU GĂSESC versiunea țintă cerută");
            break;
        case FA_DL_ATENTIE_PROBLEME_LA_DESCARCARE:
            fa_progres_actualizeaza_atentie(fai, "NU am putut descărca actualizarea");
            break;
        case FA_DL_ATENTIE_PROBLEME_LA_APLICARE:
            fa_progres_actualizeaza_atentie(fai, "NU am putut aplica actualizarea");
            break;

        case FA_DL_VERSIUNE_ESTE_ULTIMA:
            fa_progres_actualizeaza_normal(fai, "nicio versiune nouă");
            break;
        case FA_DL_AM_ACTUALIZAT_LA_VERS_TINTA:
            fa_progres_actualizeaza_normal(fai, "pachetul s-a actualizat cu succes :-)");
            break;

        case FA_DL_FINALIZARE:
            fa_progres_actualizeaza_normal(fai, "am terminat");
            gtk_widget_destroy(fai->frm);
            break;
        case FA_DL_ANULARE:
            fa_progres_actualizeaza_normal(fai, "anulez actualizarea");
            break;

        default:
            g_warning("Sunt într-un caz al switch-ului de tratare a stării automatonului actualizant care nu este tratat!");
            break;
    }

    fai->masinaActualizanta->faza = stareNoua;
}

static void fa_progres_seteaza_parametrii(FAInstanta *fai, gchar *txt) {
    g_assert(fai != NULL);
    g_assert(txt != NULL);

    gchar txtComplet[64];

    g_sprintf(txtComplet, "... %s ...", txt);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(fai->baraProgres), txtComplet);
}

static void fa_progres_actualizeaza_normal(FAInstanta *fai, gchar *txt) {
    fa_progres_seteaza_parametrii(fai, txt);
}

static void fa_progres_actualizeaza_atentie(FAInstanta *fai, gchar *txt) {
    gchar mesajSuport[70];

    g_sprintf(mesajSuport, "!! %s !!", txt);
    fa_progres_seteaza_parametrii(fai, mesajSuport);
}

static gboolean fa_proceseaza_sesiune(FAInstanta *fai) {
    g_assert(fai != NULL);

    Versiune *versLocala = NULL;
    gboolean rearmeazaMasinaDeStari = TRUE;

    switch (fai->masinaActualizanta->faza) {
        case FA_DL_INITIALIZARE:
            if ((fai->masinaActualizanta->date = dl_initializeaza(db_obtine_adresa_actualizare())) != NULL) {
                g_debug("Modulul de descărcări (downloads : 'dl') a fost inițializat cu succes.");
                fa_progres_seteaza_stare(fai, FA_DL_DESC_LISTA_ACTUALIZARI);
            } else {
                g_warning("Nu am putut inițializa modulul 'dl'! Opresc procesul de actualizare ...");
                fa_progres_seteaza_stare(fai, FA_DL_ATENTIE_MODUL_DL_NEINITIALIZAT);
            }
            break;
        case FA_DL_DESC_LISTA_ACTUALIZARI:
            if (dl_obtine_lista_actualizari(fai->masinaActualizanta->date)) {
                g_debug("Am obținut fișierul cu lista de actualizări.");
                
                fa_reseteaza_procesarea_sesiunii(fai);
                if (dl_contine_lista_actualizari_elemente(fai->masinaActualizanta->date)) {
                    fa_progres_seteaza_stare(fai, FA_DL_CANALIZEAZA_ACTIUNE);
                } else {
                    g_debug("Lista de actualizări nu conține nicio intrare!");
                    fa_progres_seteaza_stare(fai, FA_DL_ATENTIE_LISTA_ACTUALIZARI_GOALA);
                }
            } else {
                g_warning("Nu am putut accesa lista de actualizări a aplicației.");
                fa_progres_seteaza_stare(fai, FA_DL_ATENTIE_LISTA_ACTUALIZARI_NEDESCARCATA);
            }
            break;
        case FA_DL_CANALIZEAZA_ACTIUNE:
            switch (fai->parametriInvocare->tipOp) {
                case ACTUALIZEAZA_DIRECT:
                case ACTUALIZEAZA_NORMAL:
                    fa_progres_seteaza_stare(fai, FA_DL_PREGATESTE_VERSIUNE_TINTA);
                case AFISEAZA:
                    /* TODO: de gestionat caz */
                    break;
                default:
                    g_warning("Acțiunea specificată nu este recunoscută de către actualizator!");
                    fa_progres_seteaza_stare(fai, FA_DL_ATENTIE_NU_RECUNOSC_ACTIUNEA);
                    break;
            }
            break;
        case FA_DL_PREGATESTE_VERSIUNE_TINTA:
            if (fai->parametriInvocare->tipOp == ACTUALIZEAZA_DIRECT) {
                if (dl_incarca_versiune_specifica_server(fai->masinaActualizanta->date, fai->parametriInvocare->versServer) == FALSE) {
                    g_warning("Versiunea țintă dorită, '%s', nu a fost găsită în lista de versiuni disponibile! Închei procesul de actualizare ...", sda_versiune_printf(fai->parametriInvocare->versServer));
                    fa_progres_seteaza_stare(fai, FA_DL_ATENTIE_NU_GASESC_VERSIUNEA_TINTA);
                } else {
                    g_debug("Am găsit și configurat versiunea țintă dorită specificată de utilizator.");
                    fa_progres_seteaza_stare(fai, FA_DL_DESC_ACTUALIZARE);
                }
            } else {
                dl_incarca_ultima_versiune_server(fai->masinaActualizanta->date);
                g_debug("Am configurat versiunea țintă de actualizare ca fiind ultima versiune disponibilă din lista de actualizări.");
                fa_progres_seteaza_stare(fai, FA_DL_VERIFICA_VERSIUNE);
            }
            break;
        case FA_DL_VERIFICA_VERSIUNE:
            versLocala = db_obtine_versiune_curenta();
            if (dl_exista_versiune_mai_buna_decat(fai->masinaActualizanta->date, versLocala)) {
                g_debug("Versiunea țintă este mai nouă decât versiunea curentă a utilizatorului. Începe procedeul de actualizare ...");
                fa_progres_seteaza_stare(fai, FA_DL_DESC_ACTUALIZARE);
            } else {
                g_debug("Nu s-a găsit o versiune mai bună a aplicației.");
                fa_progres_seteaza_stare(fai, FA_DL_VERSIUNE_ESTE_ULTIMA);
            }
            g_free(versLocala);
            break;
        case FA_DL_DESC_ACTUALIZARE:
            if (fai->esteBtAnuleazaApasat == FALSE) {
                if ((fai->masinaActualizanta->date->adresaPachetLocal = dl_descarca_actualizare_tinta_local(fai->masinaActualizanta->date)) != NULL) {
                    g_debug("Am descărcat cu succes arhiva versiunii țintă.");
                    fa_progres_seteaza_stare(fai, FA_DL_APLICARE_ACTUALIZARE);
                } else {
                    g_warning("Nu am putut descărca versiunea țintă.");
                    fa_progres_seteaza_stare(fai, FA_DL_ATENTIE_PROBLEME_LA_DESCARCARE);
                }
            } else {
                fa_progres_seteaza_stare(fai, FA_DL_ANULARE);
            }
            break;
        case FA_DL_APLICARE_ACTUALIZARE:
            if (fai->esteBtAnuleazaApasat == FALSE) {
                if (dz_despacheteaza_pachet(fai->masinaActualizanta->date->adresaPachetLocal, DZ_CALE_DESPACHETARE)) {
                    fa_progres_seteaza_stare(fai, FA_DL_AM_ACTUALIZAT_LA_VERS_TINTA);
                } else {
                    g_warning("Nu am putut aplica actualizarea obținută!");
                    fa_progres_seteaza_stare(fai, FA_DL_ATENTIE_PROBLEME_LA_APLICARE);
                }
            } else {
                fa_progres_seteaza_stare(fai, FA_DL_ANULARE);
            }

            remove(fai->masinaActualizanta->date->adresaPachetLocal);
            break;

        case FA_DL_ATENTIE_MODUL_DL_NEINITIALIZAT:
        case FA_DL_ATENTIE_LISTA_ACTUALIZARI_NEDESCARCATA:
        case FA_DL_ATENTIE_LISTA_ACTUALIZARI_GOALA:
        case FA_DL_ATENTIE_NU_RECUNOSC_ACTIUNEA:
        case FA_DL_ATENTIE_NU_GASESC_VERSIUNEA_TINTA:
        case FA_DL_ATENTIE_PROBLEME_LA_DESCARCARE:
        case FA_DL_ATENTIE_PROBLEME_LA_APLICARE:

        case FA_DL_VERSIUNE_ESTE_ULTIMA:
        case FA_DL_AM_ACTUALIZAT_LA_VERS_TINTA:
        case FA_DL_ANULARE:
            fa_progres_seteaza_stare(fai, FA_DL_FINALIZARE);
            rearmeazaMasinaDeStari = FALSE;
            break;

        default:
            g_warning("Mașina de stări asociată procesului de actualizare nu recunoaște starea '%d'", fai->masinaActualizanta->faza);
            rearmeazaMasinaDeStari = FALSE;
            break;
    }

    return rearmeazaMasinaDeStari;
}

static gboolean fa_progres_animator(FAInstanta *fai) {
    if (fai == NULL) return FALSE;

    if(GTK_IS_PROGRESS_BAR(fai->baraProgres)) gtk_progress_bar_pulse(GTK_PROGRESS_BAR(fai->baraProgres));
    return TRUE;
}

static void fa_btAnuleaza_clicked(GtkButton *bt, FAInstanta * fai) {
    fai->esteBtAnuleazaApasat = TRUE;
    gtk_widget_set_sensitive(fai->btAnuleaza, FALSE);
    g_debug("Actualizarea a fost întreruptă la cerința directă a utilizatorului.");
}

static gboolean fa_frm_delete_event(GtkWidget *widget, GdkEvent *ev, FAInstanta * fai) {
    return FALSE;
}

static void fa_reseteaza_procesarea_sesiunii(FAInstanta *fai) {
    GSource *srcGActual = g_main_current_source();
    GMainContext *ctxtActual = g_main_context_default();
    GSource *srcGNou = g_timeout_source_new(FA_MS_INTRE_STARI);
    
    g_source_destroy(srcGActual);
    g_source_set_callback(srcGNou, (GSourceFunc)(&fa_proceseaza_sesiune), fai, NULL);
    g_source_attach(srcGNou, ctxtActual);
}