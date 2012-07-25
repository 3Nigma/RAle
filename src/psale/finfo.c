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
#include "dl.h"

#include "finfo.h"

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
  gtk_widget_hide(widget);
  return TRUE;
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
  GtkWidget *imgLicenta = gtk_image_new_from_file("media/licenta.png");
  GtkWidget *cadruImgLicenta = gtk_event_box_new();

  gtk_misc_set_alignment(GTK_MISC(lblValAutor), 1.0f, 0.1f);
  gtk_misc_set_alignment(GTK_MISC(lblValVersiune), 1.0f, 0.1f);
  gtk_misc_set_alignment(GTK_MISC(imgLicenta), 1.0f, 0.1f);
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
  //gtk_widget_set_size_request(txtView, 300, -1);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(txtView), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(txtView), GTK_WRAP_WORD);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(txtView), FALSE);
  gtk_table_attach_defaults(GTK_TABLE(cadruFrm), txtView, 2, 4, 1, 2);
}

GtkWidget *
initializeaza_formular_info() {
  GtkWidget *frm = NULL;
  GtkWidget *cadruFrm = NULL;
  GtkWidget *imgInfo = NULL;
  GtkWidget *btParasesteFrm = NULL;

  /* inițializăm formularul de informații */
  frm = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(frm), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_title(GTK_WINDOW(frm), "Despre aplicație ...");
  gtk_container_set_border_width(GTK_CONTAINER(frm), 5);
  gtk_window_set_modal(GTK_WINDOW(frm), FALSE);
  gtk_window_set_resizable(GTK_WINDOW(frm), FALSE);
  g_signal_connect(frm, "delete-event", G_CALLBACK(frmInfo_delev), NULL);

  /* inițializăm cadrul formularului */
  cadruFrm = gtk_table_new(4, 4, FALSE);
  gtk_table_set_row_spacings(GTK_TABLE(cadruFrm), 3);
  gtk_table_set_col_spacings(GTK_TABLE(cadruFrm), 2);
  gtk_container_add(GTK_CONTAINER(frm), cadruFrm);

  incarca_info_general(cadruFrm);

  /* inițializăm imaginea de informație */
  imgInfo = gtk_image_new_from_pixbuf(db_obtine_imagine_media_scalata(DB_IMG_INFO, -1, -1));
  gtk_table_attach_defaults(GTK_TABLE(cadruFrm), imgInfo, 0, 1, 0, 1);

  /* inițializăm butonul de părăsire a formularului */
  btParasesteFrm = gtk_button_new_with_label("Părăsește formular");
  gtk_button_set_relief(GTK_BUTTON(btParasesteFrm), GTK_RELIEF_HALF);
  gtk_button_set_focus_on_click(GTK_BUTTON(btParasesteFrm), FALSE);
  gtk_table_attach_defaults(GTK_TABLE(cadruFrm), btParasesteFrm, 3, 4, 3, 4);
  g_signal_connect_swapped(btParasesteFrm, "clicked", G_CALLBACK(frmInfo_delev), frm);

  return frm;
}
