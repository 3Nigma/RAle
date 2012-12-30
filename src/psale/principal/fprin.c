/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include <stdlib.h>

#include <glib.h>
#include <gdk/gdkkeysyms.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>

#ifdef G_OS_WIN32
#include <windows.h>
#endif

#include "fl.h"
#include "db.h"
#include "al.h"
#include "vc.h"
#include "fcod.h"
#include "fprin.h"

#ifndef G_VALUE_INIT 
/* îl introducem aici pentru compilarea sub windows. Se pare că GTK-ul de windows nu-l definiește */
#define G_VALUE_INIT { 0, { { 0 } } }
#endif

static gboolean frmPrincipal_la_dezapasare_taste(GtkWidget *widget, GdkEventKey *ke, FormularPrincipal *fp);
static void btIesire_clicked(GtkWidget *widget, FormularPrincipal *fp);

static gboolean estePlacutaConectata;
static Limbaj lmbCodExemple = ASM;
static gboolean dlgCodForteazaActualizConex;
static VizualizatorCartulie *vizAle;

static void placuta_sa_deconectat() {
    dlgCodForteazaActualizConex = TRUE;
}

static gboolean imgLogo_click(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
#ifdef G_OS_WIN32
    ShellExecute(NULL, "open", "http://tuscale.ro", NULL, NULL, SW_SHOWNORMAL);
#elif defined G_OS_UNIX
    system("xdg-open 'http://tuscale.ro' &");
#endif

    return TRUE;
}

static void cmbxCodNou_selectat(GtkComboBox *widget, FormularPrincipal *fp) {
    /* interpretăm selecția doar dacă ea este una validă (se dorește un limbaj valid) */
    if (gtk_combo_box_get_active(widget) != 0) {
        FormularCod *dlgCod = NULL;

        switch (gtk_combo_box_get_active(widget)) {
            case 1: /* ASM */
                dlgCod = fc_initializeaza_fara_cod(ASM);
                break;
            case 2: /* C */
                dlgCod = fc_initializeaza_fara_cod(C);
                break;
        }
        dlgCod->laDepistare_neprezentaPlacuta_recurenta = &placuta_sa_deconectat;
        fp->listaDlgCod = g_slist_prepend(fp->listaDlgCod, dlgCod);

        /* repoziționăm textul afișat */
        gtk_combo_box_set_active(GTK_COMBO_BOX(widget), 0);

        fc_actualizeaza_stare_placuta(dlgCod->lblStareConex, estePlacutaConectata, TRUE);
        fc_modifica_vizibilitate(dlgCod, TRUE);
    }
}

static void cmbxCodCarte_la_selectie(GtkComboBox *widget, FormularPrincipal *fp) {
    FormularCod *dlgCod = NULL;
    GtkTreeIter iter;
    GtkTreeModel *model = NULL;
    gchar *titluScurt = NULL;
    gchar *titluLung;

    if (gtk_combo_box_get_active_iter(widget, &iter)) {
        model = gtk_combo_box_get_model(widget);
        gtk_tree_model_get(model, &iter, 0, &titluScurt, 1, &titluLung, -1);

        if (g_strcmp0(titluScurt, "") != 0) {
            dlgCod = fc_initializeaza_cu_exemplu(titluScurt, titluLung);
            dlgCod->laDepistare_neprezentaPlacuta_recurenta = &placuta_sa_deconectat;
            fp->listaDlgCod = g_slist_prepend(fp->listaDlgCod, dlgCod);

            /* repoziționăm textul afișat */
            gtk_combo_box_set_active(GTK_COMBO_BOX(widget), 0);

            fc_actualizeaza_stare_placuta(dlgCod->lblStareConex, estePlacutaConectata, TRUE);
            fc_modifica_vizibilitate(dlgCod, TRUE);
        }
    }
}

static void btCartulie_click(GtkWidget *widget, FormularPrincipal *fp) {
    if (NULL == vizAle) {
        vizAle = vc_initializeaza();

        if (NULL != vizAle) {
            vc_sari_la_pagina(vizAle, 0);
            gtk_widget_show_all(vizAle->frm);
        }
    } else {
        gtk_widget_show(GTK_WIDGET(vizAle->frm));
    }
}

static void btInfo_click(GtkWidget *widget, FormularPrincipal *fp) {
    g_assert(fp != NULL);
    
    if(fp->infoAplicatie == NULL) {
       fp->infoAplicatie = finfo_initializeaza(GTK_WINDOW(fp->frm));
    }

    if (fp->infoAplicatie != NULL) {
        finfo_arata(fp->infoAplicatie);
    } else {
        g_warning("Nu am putut arăta formularul info pentru că el nu s-a putut aloca!");
    }
}

static gboolean frmPrincipal_delev(GtkWidget *widget, GdkEvent *event, gpointer data) {

    return FALSE;
}

static void frmPrincipal_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

static GtkWidget *creeaza_cmbxExemple() {
    GtkWidget *deRet = NULL;
    GtkListStore *magazie = NULL;
    GtkCellRenderer *afisorCel0 = NULL;
    GtkCellRenderer *afisorCel1 = NULL;
    GValue val = G_VALUE_INIT;

    magazie = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    deRet = gtk_combo_box_new_with_model(GTK_TREE_MODEL(magazie));
    g_object_unref(G_OBJECT(magazie));

    /* adăugăm interpretoarele vizuale de text */
    g_value_init(&val, G_TYPE_STRING);

    afisorCel0 = gtk_cell_renderer_text_new();
    g_value_set_static_string(&val, "Sans Italic 8");
    g_object_set_property(G_OBJECT(afisorCel0), "font", &val);
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(deRet), afisorCel0, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(deRet), afisorCel0, "text", 0, NULL);

    afisorCel1 = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(deRet), afisorCel1, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(deRet), afisorCel1, "text", 1, NULL);

    return deRet;
}

static void populeaza_cmbxExemple(GtkComboBox *cmbx, Limbaj lmb) {
    g_assert(cmbx != NULL);

    GtkListStore *magazie = NULL;
    GtkTreeIter iter;

    /* inițializăm modelul și adăugăm capul de coloană */
    magazie = GTK_LIST_STORE(gtk_combo_box_get_model(cmbx));
    gtk_list_store_clear(magazie);
    gtk_list_store_append(magazie, &iter);
    switch (lmb) {
        case C:
            db_incarca_exemple_carte(magazie, "c");
            gtk_list_store_set(magazie, &iter, 0, "", 1, "Cod din cărțulie ....", -1);
            break;
        case ASM:
            db_incarca_exemple_carte(magazie, "s");
            gtk_list_store_set(magazie, &iter, 0, "", 1, "Cod din cărțulie ...", -1);
            break;
    }
    gtk_combo_box_set_active(cmbx, 0);
}

static gboolean la_interval_tick(FormularPrincipal *fp) {
    gboolean stareConexCurenta;

    if (dlgCodForteazaActualizConex ||
            (estePlacutaConectata == FALSE && (stareConexCurenta = al_este_placuta_conectata()))) {
        GSList *elemFCod = fp->listaDlgCod;
        FormularCod *fc = NULL;

        if (dlgCodForteazaActualizConex)
            stareConexCurenta = al_este_placuta_conectata();
        while (NULL != elemFCod) {
            fc = (FormularCod *) elemFCod->data;
            if (GTK_IS_WIDGET(fc->frm) != FALSE) {
                fc_actualizeaza_stare_placuta(fc->lblStareConex, stareConexCurenta, FALSE);
            }
            elemFCod = g_slist_next(elemFCod);
        }

        estePlacutaConectata = stareConexCurenta;
        dlgCodForteazaActualizConex = FALSE;
    }

    return TRUE;
}

static gboolean frmPrincipal_la_dezapasare_taste(GtkWidget *widget, GdkEventKey *ke, FormularPrincipal *fp) {
    gboolean evGestionat = FALSE;

    if (evGestionat == FALSE &&
            (ke->state & GDK_CONTROL_MASK) != 0) {
        /* tasta 'Ctrl' a fost apăsată */
        switch (ke->keyval) {
            case GDK_KEY_P:
            case GDK_KEY_p:
                /* ... și tasta 'p'. Ieși din aplicație. */
                btIesire_clicked(widget, fp);
                evGestionat = TRUE;
                break;
            case GDK_KEY_S:
            case GDK_KEY_s:
                if (ASM == lmbCodExemple) {
                    lmbCodExemple = C;
                } else {
                    lmbCodExemple = ASM;
                }
                populeaza_cmbxExemple(GTK_COMBO_BOX(fp->cmbxExemple), lmbCodExemple);
                evGestionat = TRUE;
                break;
            case GDK_KEY_V:
            case GDK_KEY_v:
                /* ... și tasta 'v'. Deschide vizualizatorul cărțuliei. */
                btCartulie_click(widget, fp);
                evGestionat = TRUE;
                break;
        }
    } else if (ke->keyval == GDK_KEY_F1) {
        /* tasta 'F1' a fost apăsată. Afișează formularul de informații. */
        btInfo_click(widget, fp);
        evGestionat = TRUE;
    }

    return evGestionat;
}

static void btIesire_clicked(GtkWidget *widget, FormularPrincipal *fp) {
    g_assert(NULL != fp);

    gtk_widget_destroy(GTK_WIDGET(fp->frm));
}

FormularPrincipal *fp_initializeaza_formular_principal() {
    FormularPrincipal *fp = NULL;
    GtkWidget *cadruFormPrincipal = NULL;
    GtkWidget *frm = NULL;
    GtkWidget *cadruImgLogo = NULL;
    GtkWidget *imgLogo = NULL;
    GtkWidget *cmbxCodNou = NULL;
    GtkWidget *cmbxCodCarte = NULL;
    GtkWidget *btCartulie = NULL;
    GtkWidget *btInfo = NULL;
    GtkWidget *btIesire = NULL;

    /* inițializăm alte module */
    db_initializeaza();

    /* inițializăm formularul principal */
    frm = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(frm), 8);
    gtk_window_set_icon(GTK_WINDOW(frm), fl_obtine_imagine_media_scalata(FL_GENERAL_IMG_FORMULARE, -1, -1));
    gtk_window_set_resizable(GTK_WINDOW(frm), FALSE);
    gtk_window_set_title(GTK_WINDOW(frm), "psAle");
    gtk_window_set_position(GTK_WINDOW(frm), GTK_WIN_POS_CENTER);

    /* inițializăm cadrul formularului principal */
    cadruFormPrincipal = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(frm), cadruFormPrincipal);

    /* inițializăm logoul aplicației */
    imgLogo = gtk_image_new_from_pixbuf(fl_obtine_imagine_media_scalata(FL_IMG_TUSCALE_LOGO, -1, -1));

    /* inițializăm cadrul pentru logoul aplicației */
    cadruImgLogo = gtk_event_box_new();
    gtk_widget_set_tooltip_markup(cadruImgLogo, "Imaginea proiectului ...\n<i>(Click pe ea pentru a deschide pagina <b>tuScale</b>)</i>");
    gtk_widget_set_events(cadruImgLogo, GDK_KEY_PRESS_MASK);
    gtk_container_add(GTK_CONTAINER(cadruImgLogo), imgLogo);
    gtk_container_add(GTK_CONTAINER(cadruFormPrincipal), cadruImgLogo);

    /* inițializăm meniul de selecție pentru programare brută */
    cmbxCodNou = gtk_combo_box_text_new();
    gtk_widget_set_tooltip_markup(cmbxCodNou, "Deschide un formular de cod nou utilizând limbajul dorit.");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbxCodNou), "\t\t\tHai la programare ...");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbxCodNou), "\t\t\t\t\t\t... în ASM");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbxCodNou), "\t\t\t\t\t\t... în C");
    gtk_combo_box_set_active(GTK_COMBO_BOX(cmbxCodNou), 0);
    gtk_container_add(GTK_CONTAINER(cadruFormPrincipal), cmbxCodNou);

    /* inițializăm meniul de selecție pentru exemplele de cod din carte */
    cmbxCodCarte = creeaza_cmbxExemple();
    populeaza_cmbxExemple(GTK_COMBO_BOX(cmbxCodCarte), lmbCodExemple);
    gtk_widget_set_tooltip_markup(cmbxCodCarte, "Navighează și deschide exemplele din cărțulie.\nTaste scurte pentru a trece în celălalt limbaj [C/S]: <i>Ctrl + S</i>");
    gtk_container_add(GTK_CONTAINER(cadruFormPrincipal), cmbxCodCarte);

    /* inițializăm butonul de citire a cărțuliei */
    btCartulie = gtk_button_new_with_label("Vizitează cărțulia");
    gtk_widget_set_tooltip_markup(btCartulie, "Deschide cărțulia în format electronic.\nTaste scurte: <i>Ctrl + V</i>");
    gtk_button_set_relief(GTK_BUTTON(btCartulie), GTK_RELIEF_HALF);
    gtk_button_set_focus_on_click(GTK_BUTTON(btCartulie), FALSE);
    gtk_container_add(GTK_CONTAINER(cadruFormPrincipal), btCartulie);

    /* inițializăm butonul de informații */
    btInfo = gtk_button_new_with_label("Ce avem aici?");
    gtk_widget_set_tooltip_markup(btInfo, "Afișează informații generale despre '<i>psAle</i>'.\nTastă scurtă: <i>F1</i>");
    gtk_button_set_relief(GTK_BUTTON(btInfo), GTK_RELIEF_NONE);
    gtk_button_set_focus_on_click(GTK_BUTTON(btInfo), FALSE);
    gtk_container_add(GTK_CONTAINER(cadruFormPrincipal), btInfo);

    /* inițializăm butonul de ieșire */
    btIesire = gtk_button_new_with_label("Gata, am ieșit!");
    gtk_widget_set_tooltip_markup(btIesire, "Părăsește definitiv aplicația.\nTaste scurte: <i>Ctrl + P</i>");
    gtk_button_set_relief(GTK_BUTTON(btIesire), GTK_RELIEF_NONE);
    gtk_button_set_focus_on_click(GTK_BUTTON(btIesire), FALSE);
    gtk_container_add(GTK_CONTAINER(cadruFormPrincipal), btIesire);

    /* permite butoanelor să afișeze atât imaginea cât și textul dorit */
    GtkSettings *default_settings = gtk_settings_get_default();
    g_object_set(default_settings, "gtk-button-images", TRUE, NULL);

    /* inițializează structura necesară */
    fp = g_new0(FormularPrincipal, 1);
    fp->frm = frm;
    fp->cmbxExemple = cmbxCodCarte;

    /* leagă evenimentele */
    g_signal_connect(frm, "key-release-event", G_CALLBACK(frmPrincipal_la_dezapasare_taste), fp);
    g_signal_connect(frm, "delete-event", G_CALLBACK(frmPrincipal_delev), NULL);
    g_signal_connect(frm, "destroy", G_CALLBACK(frmPrincipal_destroy), NULL);
    g_signal_connect(cadruImgLogo, "button-press-event", G_CALLBACK(imgLogo_click), NULL);
    g_signal_connect(cmbxCodNou, "changed", G_CALLBACK(cmbxCodNou_selectat), fp);
    g_signal_connect(cmbxCodCarte, "changed", G_CALLBACK(cmbxCodCarte_la_selectie), fp);
    g_signal_connect(btCartulie, "clicked", G_CALLBACK(btCartulie_click), fp);
    g_signal_connect(btInfo, "clicked", G_CALLBACK(btInfo_click), fp);
    g_signal_connect(btIesire, "clicked", G_CALLBACK(btIesire_clicked), fp);

    /* dăm drumul la tick-ul de interval */
    estePlacutaConectata = FALSE;
    dlgCodForteazaActualizConex = FALSE;
    g_timeout_add_seconds_full(G_PRIORITY_DEFAULT, 1, (GSourceFunc) (la_interval_tick), fp, NULL);

    return fp;
}

void fp_arata(FormularPrincipal *fp) {
    g_assert(NULL != fp);

    gtk_widget_show_all(fp->frm);
}

void fp_curata(FormularPrincipal *fp) {
    if(fp == NULL) return;
    
    /* curățăm valori locale */
    g_slist_free(fp->listaDlgCod);
    finfo_curata(&fp->infoAplicatie);
    g_free(fp);

    /* eliberăm alte module utilizate */
    db_curata();
}
