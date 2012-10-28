/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include "feeprom.h"

#include <glib/gprintf.h>

static gboolean frmEEPROM_delev(GtkWidget *widget, GdkEvent *event, FormularEEPROM *fe);
static GtkWidget *construieste_vizualizator_memorie();

FormularEEPROM *
fme_initializeaza(GtkWindow *parinte) {
  FormularEEPROM *fe = NULL;
  GtkWidget *zonaButoane = NULL;
  GtkWidget *btCitesteMem = NULL;
  GtkWidget *btScrieMem = NULL;
  GtkWidget *btParasesteFrm = NULL;
  GtkWidget *zonaGenerala = NULL;
  
  fe = g_new(FormularEEPROM, 1);
  
  /* creează formularul principal */
  fe->frm = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(fe->frm), "Acțiuni memorie specială");
  gtk_window_set_transient_for(GTK_WINDOW(fe->frm), parinte);
  gtk_window_set_position(GTK_WINDOW(fe->frm), GTK_WIN_POS_CENTER_ON_PARENT);
  zonaGenerala = gtk_vbox_new(FALSE, 3);
  
  /* creează tabelul de valori a memoriei speciale */
  fe->tvEEPROM = construieste_vizualizator_memorie();
  gtk_widget_set_size_request(fe->tvEEPROM, 455, 250);
  gtk_box_pack_start(GTK_BOX(zonaGenerala), fe->tvEEPROM, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(zonaGenerala), gtk_hseparator_new(), FALSE, FALSE, 0);
  
  /* creează butoanele formularului */
  zonaButoane = gtk_hbutton_box_new();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(zonaButoane), GTK_BUTTONBOX_END);
  gtk_box_pack_start(GTK_BOX(zonaGenerala), zonaButoane, FALSE, FALSE, 0);
  
  btCitesteMem = gtk_button_new_with_label("Citește EEPROM");
  gtk_box_pack_start(GTK_BOX(zonaButoane), btCitesteMem, FALSE, FALSE, 0);
  
  btScrieMem = gtk_button_new_with_label("Scrie EEPROM");
  gtk_box_pack_start(GTK_BOX(zonaButoane), btScrieMem, FALSE, FALSE, 0);
  
  btParasesteFrm = gtk_button_new_with_label("Închide formular");
  gtk_box_pack_start(GTK_BOX(zonaButoane), btParasesteFrm, FALSE, FALSE, 0);

  /* atașează totul la formular */
  gtk_container_set_border_width(GTK_CONTAINER(fe->frm), 4);
  gtk_container_add(GTK_CONTAINER(fe->frm), zonaGenerala);
  
  /* legăm evenimentele necesare */
  g_signal_connect(fe->frm, "delete-event", G_CALLBACK(frmEEPROM_delev), (gpointer)fe);

  return fe;
}

void 
fme_afiseaza(FormularEEPROM *fe) {
  gtk_widget_show_all(GTK_WIDGET(fe->frm));
}

static GtkWidget *
construieste_vizualizator_memorie() {
  GtkWidget *vizMem = NULL;
  GtkTreeViewColumn *clmTreeView = NULL;
  GtkListStore *lstModel = NULL;
  GtkCellRenderer *txtCellRenderer = NULL;
  gint cnt = 0;
  gchar capColoana[5];
  
  vizMem = gtk_tree_view_new();
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(vizMem), TRUE);
  
  for(cnt = 0; cnt < 17; ++ cnt) {
    clmTreeView = gtk_tree_view_column_new();
    txtCellRenderer = gtk_cell_renderer_text_new();
    if(cnt != 0) {
      g_sprintf(capColoana, "%02X", cnt - 1);
      g_object_set(txtCellRenderer, "editable", TRUE, NULL);
    } else {
	  g_sprintf(capColoana, "    ");
	}
	gtk_tree_view_column_set_title(clmTreeView, capColoana);
    gtk_tree_view_column_pack_start(clmTreeView, txtCellRenderer, FALSE);
    gtk_tree_view_column_add_attribute(clmTreeView, txtCellRenderer, "text", cnt);
    gtk_tree_view_append_column(GTK_TREE_VIEW(vizMem), clmTreeView);
  }
  
  lstModel = gtk_list_store_new(17, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                                    G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                                    G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                                    G_TYPE_STRING, G_TYPE_STRING);
  al_citeste_eeprom(lstModel);
  gtk_tree_view_set_model(GTK_TREE_VIEW(vizMem), GTK_TREE_MODEL(lstModel));
  
  return vizMem;
}

static gboolean 
frmEEPROM_delev(GtkWidget *widget, GdkEvent *event, FormularEEPROM *fe) {
  g_free(fe);
  return FALSE;
}
