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

#include "fl.h"
#include "db.h"
#include "dl.h"

#include "finfo.h"

/* ATENȚIE: a nu se muta această incluziune în altă parte pentru că G_OS_WIN32 se definește în gtk/gtk.h care este inclus în 'finfo.h' */
#if defined(G_OS_WIN32) && !defined(_WINDOWS_H)
  #include <windows.h>
#endif

static void incarca_info_general(GtkWidget *cadruFrm);
static gboolean frmInfo_delev(GtkWidget *widget, GdkEvent *event, gpointer data);
static gboolean frmInfo_la_dezapasare_taste(GtkWidget *widget, GdkEventKey *ke, GtkWindow *fereastraParinte);
static void btIesire_clicked(GtkWidget *widget, GtkWindow *fereastraParinte);

GtkWidget *
finfo_initializeaza(GtkWindow *parinte) {
  GtkWidget *frm = NULL;
  GtkWidget *cadruFrm = NULL;
  GtkWidget *imgInfo = NULL;
  GtkWidget *btParasesteFrm = NULL;
  
  /* inițializăm formularul de informații */
  frm = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(frm), "psAle ~ Despre aplicație ...");
  gtk_window_set_transient_for(GTK_WINDOW(frm), parinte);
  gtk_window_set_position(GTK_WINDOW(frm), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_container_set_border_width(GTK_CONTAINER(frm), 5);
  gtk_window_set_modal(GTK_WINDOW(frm), FALSE);
  gtk_window_set_resizable(GTK_WINDOW(frm), FALSE);
  g_signal_connect(frm, "key-release-event", G_CALLBACK(frmInfo_la_dezapasare_taste), frm);
  g_signal_connect(frm, "delete-event", G_CALLBACK(frmInfo_delev), NULL);

  /* inițializăm cadrul formularului */
  cadruFrm = gtk_table_new(4, 4, FALSE);
  gtk_table_set_row_spacings(GTK_TABLE(cadruFrm), 3);
  gtk_table_set_col_spacings(GTK_TABLE(cadruFrm), 2);
  gtk_container_add(GTK_CONTAINER(frm), cadruFrm);

  incarca_info_general(cadruFrm);

  /* inițializăm imaginea de informație */
  GdkPixbuf *pixInfoPictograma = fl_obtine_imagine_media_scalata(FL_IMG_FINFO_INFO, -1, -1);
  imgInfo = gtk_image_new_from_pixbuf(pixInfoPictograma);
  g_object_unref(pixInfoPictograma);
  gtk_table_attach_defaults(GTK_TABLE(cadruFrm), imgInfo, 0, 1, 0, 1);

  /* inițializăm butonul de părăsire a formularului */
  btParasesteFrm = gtk_button_new_with_label("Părăsește formular");
  gtk_button_set_relief(GTK_BUTTON(btParasesteFrm), GTK_RELIEF_HALF);
  gtk_button_set_focus_on_click(GTK_BUTTON(btParasesteFrm), FALSE);
  gtk_widget_set_tooltip_markup(btParasesteFrm, "Închide formularul de informații.\nTastă scurtă: <i>Esc</i>");
  gtk_table_attach_defaults(GTK_TABLE(cadruFrm), btParasesteFrm, 3, 4, 3, 4);
  g_signal_connect(btParasesteFrm, "clicked", G_CALLBACK(btIesire_clicked), frm);

  /* atașează pictogramă la acțiunea de părăsire a formularului */
  GdkPixbuf *imgParasesteFrmPixBuf = fl_obtine_imagine_media_scalata(FL_IMG_FINFO_PARASESTE, 16, 16);
  GtkWidget *imgParasesteFrm = gtk_image_new_from_pixbuf(imgParasesteFrmPixBuf);
  g_object_unref(imgParasesteFrmPixBuf);
  gtk_button_set_image_position(GTK_BUTTON(btParasesteFrm), GTK_POS_RIGHT);
  gtk_button_set_image(GTK_BUTTON(btParasesteFrm), imgParasesteFrm);
  
  return frm;
}

static gboolean 
imgLicenta_click(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
#ifdef G_OS_WIN32
  ShellExecute(NULL, "open", "http://creativecommons.org/licenses/by-nc-sa/3.0/", NULL, NULL, SW_SHOWNORMAL);
#elif defined G_OS_UNIX
  system("xdg-open 'http://creativecommons.org/licenses/by-nc-sa/3.0/' &");
#endif

  return TRUE;
}

static gboolean 
frmInfo_delev(GtkWidget *widget, GdkEvent *event, gpointer data) {
  return FALSE;
}

static gboolean 
frmInfo_la_dezapasare_taste(GtkWidget *widget, GdkEventKey *ke, GtkWindow *fereastraParinte) {
  gboolean evGestionat = FALSE;
  
  if(ke->keyval == GDK_KEY_Escape) {
	/* tasta 'Esc' a fost apăsată. Părăsește formularul. */
    btIesire_clicked(widget, fereastraParinte);
    evGestionat = TRUE;
  }
  
  return evGestionat;
}

static void 
btIesire_clicked(GtkWidget *widget, GtkWindow *fereastraParinte) {
  if(NULL != fereastraParinte) {
    gtk_widget_destroy(GTK_WIDGET(fereastraParinte));
  }
}

static void 
incarca_info_general(GtkWidget *cadruFrm) {
  GtkWidget *cadruTitluEtichete = NULL;
  GtkWidget *cadruValEtichete = NULL;

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
  GtkWidget *lblValVersiune = gtk_label_new(PSALE_VERSIUNE);
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

  GtkWidget *txtView = gtk_text_view_new();
  GtkTextBuffer *txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(txtView));

  gtk_text_buffer_set_text(txtBuffer, PSALE_TEXT_DESPRE, -1);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(txtView), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(txtView), GTK_WRAP_WORD);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(txtView), FALSE);
  gtk_table_attach_defaults(GTK_TABLE(cadruFrm), txtView, 2, 4, 1, 2);
}
