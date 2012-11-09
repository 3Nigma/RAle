/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADASCALI?EI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include "feeprom.h"

#include "fl.h"

#include <gdk/gdkkeysyms.h>
#include <glib/gprintf.h>
#include <string.h>

#define CULOARE_COLOANA_ADRESE_MUL_HEX "#F0F0F0"

static gboolean frmEEPROM_delev(GtkWidget *widget, GdkEvent *event, FormularEEPROM *fe);
static GtkWidget *construieste_vizualizator_memorie();
static gboolean laArataTooltip(GtkWidget  *widget,
                                gint        x,
                                gint        y,
                                gboolean    keyboard_mode,
                                GtkTooltip *indicatiiCelula,
                                FormularEEPROM *fe);
static gboolean frmEEPROM_la_dezapasare_taste(GtkWidget *widget, GdkEventKey *ke, FormularEEPROM *fe);
static gboolean tree_view_get_cell_from_pos(GtkTreeView *view, guint x, guint y, 
                                             guint *cellx, guint *celly);
static void laEditareCelula(GtkCellRendererText *renderer,
                             gchar *path, gchar *new_text,
                             GtkTreeView *tv);
static void btCitesteEEPROM_click(GtkWidget *widget, FormularEEPROM *fe);
static void btSalveazaEEPROM_click(GtkWidget *widget, FormularEEPROM *fe);
static void btParasesteFrm_click(GtkWidget *widget, FormularEEPROM *fe);

static gboolean esteHexValid(gchar *str);
                                                        
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
  gtk_window_set_title(GTK_WINDOW(fe->frm), "psAle ~ Acțiuni ale memoriei speciale");
  gtk_window_set_icon(GTK_WINDOW(fe->frm), fl_obtine_imagine_media_scalata(FL_GENERAL_IMG_FORMULARE, -1, -1));
  gtk_window_set_transient_for(GTK_WINDOW(fe->frm), parinte);
  gtk_window_set_position(GTK_WINDOW(fe->frm), GTK_WIN_POS_CENTER_ON_PARENT);
  zonaGenerala = gtk_vbox_new(FALSE, 3);
  
  /* creează tabelul de valori a memoriei speciale */
  fe->tvEEPROM = construieste_vizualizator_memorie();
#ifdef G_OS_WIN32
  gtk_widget_set_size_request(fe->tvEEPROM, 418, 250);
#elif defined G_OS_UNIX
  gtk_widget_set_size_request(fe->tvEEPROM, 455, 250);
#endif
  gtk_box_pack_start(GTK_BOX(zonaGenerala), fe->tvEEPROM, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(zonaGenerala), gtk_hseparator_new(), FALSE, FALSE, 0);
  
  /* creează butoanele formularului */
  zonaButoane = gtk_hbutton_box_new();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(zonaButoane), GTK_BUTTONBOX_END);
  gtk_box_pack_start(GTK_BOX(zonaGenerala), zonaButoane, FALSE, FALSE, 0);
  
  btCitesteMem = gtk_button_new_with_label("Citește EEPROM");
  gtk_button_set_relief(GTK_BUTTON(btCitesteMem), GTK_RELIEF_HALF);
  gtk_widget_set_tooltip_markup(btCitesteMem, "Citește starea curentă a memoriei speciale.\nTaste scurte: <i>Ctrl + C</i>");
  GdkPixbuf *imgCitesteMemPixBuf = fl_obtine_imagine_media_scalata(FL_IMG_FEEPROM_CITESTE, 16, 16);
  GtkWidget *imgCitesteMem = gtk_image_new_from_pixbuf(imgCitesteMemPixBuf);
  g_object_unref(imgCitesteMemPixBuf);
  gtk_button_set_image_position(GTK_BUTTON(btCitesteMem), GTK_POS_LEFT);
  gtk_button_set_image(GTK_BUTTON(btCitesteMem), imgCitesteMem);
  gtk_box_pack_start(GTK_BOX(zonaButoane), btCitesteMem, FALSE, FALSE, 0);
  
  btScrieMem = gtk_button_new_with_label("Scrie EEPROM");
  gtk_button_set_relief(GTK_BUTTON(btScrieMem), GTK_RELIEF_HALF);
  gtk_widget_set_tooltip_markup(btScrieMem, "Scrie memoria specială cu valorile dorite.\nTaste scurte: <i>Ctrl + S</i>");
  GdkPixbuf *imgbtScrieMemPixBuf = fl_obtine_imagine_media_scalata(FL_IMG_FEEPROM_SCRIE, 16, 16);
  GtkWidget *imgbtScrieMem = gtk_image_new_from_pixbuf(imgbtScrieMemPixBuf);
  g_object_unref(imgbtScrieMemPixBuf);
  gtk_button_set_image_position(GTK_BUTTON(btScrieMem), GTK_POS_LEFT);
  gtk_button_set_image(GTK_BUTTON(btScrieMem), imgbtScrieMem);
  gtk_box_pack_start(GTK_BOX(zonaButoane), btScrieMem, FALSE, FALSE, 0);
  
  btParasesteFrm = gtk_button_new_with_label("Închide formular");
  gtk_button_set_relief(GTK_BUTTON(btParasesteFrm), GTK_RELIEF_HALF);
  gtk_widget_set_tooltip_markup(btParasesteFrm, "Închide formularul curent.\nTastă scurta: <i>Esc</i>");
  GdkPixbuf *imgParasesteFrmPixBuf = fl_obtine_imagine_media_scalata(FL_IMG_FEEPROM_PARASESTE, 16, 16);
  GtkWidget *imgParasesteFrm = gtk_image_new_from_pixbuf(imgParasesteFrmPixBuf);
  g_object_unref(imgParasesteFrmPixBuf);
  gtk_button_set_image_position(GTK_BUTTON(btParasesteFrm), GTK_POS_RIGHT);
  gtk_button_set_image(GTK_BUTTON(btParasesteFrm), imgParasesteFrm);
  gtk_box_pack_start(GTK_BOX(zonaButoane), btParasesteFrm, FALSE, FALSE, 0);
  
  /* atașează totul la formular */
  gtk_container_set_border_width(GTK_CONTAINER(fe->frm), 4);
  gtk_container_add(GTK_CONTAINER(fe->frm), zonaGenerala);
  
  /* legagă evenimentele necesare */
  g_signal_connect(fe->frm, "delete-event", G_CALLBACK(frmEEPROM_delev), (gpointer)fe);
  g_signal_connect(fe->frm, "key-release-event", G_CALLBACK(frmEEPROM_la_dezapasare_taste), (gpointer)fe);
  g_signal_connect(fe->tvEEPROM, "query-tooltip", G_CALLBACK(laArataTooltip), (gpointer)fe);
  g_signal_connect(btCitesteMem, "clicked", G_CALLBACK(btCitesteEEPROM_click), (gpointer)fe);
  g_signal_connect(btScrieMem, "clicked", G_CALLBACK(btSalveazaEEPROM_click), (gpointer)fe);
  g_signal_connect(btParasesteFrm, "clicked", G_CALLBACK(btParasesteFrm_click), (gpointer)fe);
  
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
  gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(vizMem), TRUE);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(vizMem), TRUE);
  gtk_widget_set_has_tooltip(vizMem, TRUE);
  
  for(cnt = 0; cnt < 17; ++ cnt) {
    clmTreeView = gtk_tree_view_column_new();
    txtCellRenderer = gtk_cell_renderer_text_new();
    g_object_set_data(G_OBJECT(txtCellRenderer), "x-coord", GINT_TO_POINTER(cnt));
    if(cnt != 0) {
      g_sprintf(capColoana, "%02X", cnt - 1);
      g_object_set(txtCellRenderer, "editable", TRUE, NULL);
    } else {
	  g_sprintf(capColoana, "    ");
	  g_object_set(txtCellRenderer, "background", CULOARE_COLOANA_ADRESE_MUL_HEX, NULL);
	}
	gtk_tree_view_column_set_title(clmTreeView, capColoana);
    gtk_tree_view_column_pack_start(clmTreeView, txtCellRenderer, FALSE);
    gtk_tree_view_column_add_attribute(clmTreeView, txtCellRenderer, "text", cnt);
    gtk_tree_view_append_column(GTK_TREE_VIEW(vizMem), clmTreeView);
    
    /* leagă evenimentele necesare */
    g_signal_connect(txtCellRenderer, "edited", G_CALLBACK(laEditareCelula), vizMem);
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

static gboolean 
laArataTooltip(GtkWidget  *widget,
               gint        x,
               gint        y,
               gboolean    keyboard_mode,
               GtkTooltip *indicatiiCelula,
               FormularEEPROM *fe) {
  gint relBinX;
  gint relBinY;
  guint cellX;
  guint cellY;
  GtkTreePath *caleCelula;
  gchar ttText[512];
  
  /* obține coordonate relative hover */
  gtk_tree_view_convert_widget_to_bin_window_coords(GTK_TREE_VIEW(fe->tvEEPROM), x, y, &relBinX, &relBinY);
  gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(fe->tvEEPROM), relBinX, relBinY, &caleCelula, NULL,
                                NULL, NULL); /* cell_x, cell_y */
 
  if(tree_view_get_cell_from_pos(GTK_TREE_VIEW(fe->tvEEPROM), relBinX, relBinY, &cellX, &cellY) == TRUE) {
	if(cellX > 0 ) {
      g_sprintf(ttText, "<b>%s</b>0x%03x"/*\n<b>%s</b>%s\n<b>%s</b>%s"*/, 
                        "Adresa: ", cellY * 16 + cellX - 1/*, 
                        "Val. zecimala: ", "DE COMPLETAT", 
                        "Val. binara: ", "DE COMPLETAT"*/);
      gtk_tooltip_set_markup(indicatiiCelula, ttText);
      gtk_tree_view_set_tooltip_cell(GTK_TREE_VIEW(fe->tvEEPROM), indicatiiCelula, caleCelula, NULL, NULL);
    
      return TRUE;
    }
  }
  
  return FALSE;
}

static gboolean 
frmEEPROM_la_dezapasare_taste(GtkWidget *widget, GdkEventKey *ke, FormularEEPROM *fe) {
  gboolean evGestionat = FALSE;
  
  if((ke->state & GDK_CONTROL_MASK) != 0) {
    /* tasta 'Ctrl' a fost apasata */
    switch(ke->keyval) {
    case GDK_KEY_S:
    case GDK_KEY_s:
      /* și tasta 's' a fost apăsată. Scrie memoria EEPROM */
      btSalveazaEEPROM_click(widget, fe);
      break;
    case GDK_KEY_C:
    case GDK_KEY_c:
      /* și tasta 'c' a fost apăsată. Citește starea memoriei EEPROM */
      btCitesteEEPROM_click(widget, fe);
      break;
    }
  } else if(ke->keyval == GDK_KEY_Escape) {
    btParasesteFrm_click(widget, fe);
  }
  
  return evGestionat;
}

static gboolean
tree_view_get_cell_from_pos(GtkTreeView *view, guint x, guint y, 
                            guint *cellx, guint *celly) {
  GtkTreeViewColumn *col = NULL;
  GList *node, *columns, *cells;
  guint colx = 0;
  guint coly = 0;
  guint internalCellX = 0;
  guint internalCellY = 0;
  gint width = 0;
  gint height = 0;
  gint n_linii = 0;
  gint cnt = 0;
  
  g_return_val_if_fail(view != NULL, FALSE);
  g_return_val_if_fail(cellx != NULL, FALSE);
  g_return_val_if_fail(celly != NULL, FALSE);

  /* găsește coloana potrivită */
  columns = gtk_tree_view_get_columns(view);
  for(node = columns;  node != NULL && col == NULL;  node = node->next) {
    GtkTreeViewColumn *checkcol = (GtkTreeViewColumn*) node->data;

    if(x >= colx  &&  x < (colx + checkcol->width)) {
      col = checkcol;
      (*cellx) = internalCellX;
    } else {
      colx += checkcol->width;
      internalCellX++;
    }
  }
  g_list_free(columns);

  if(col == NULL) {
    return FALSE; /* nu am găsit nici un candidat */
  }
	
  /* găsește celula potrivită în coloana respectivă */
  n_linii = gtk_tree_model_iter_n_children(gtk_tree_view_get_model(view), NULL);
  cells = gtk_tree_view_column_get_cell_renderers(col);
  GtkCellRenderer *checkcell = (GtkCellRenderer*)cells->data;
  gtk_cell_renderer_get_size(checkcell, GTK_WIDGET(view), NULL, NULL, NULL, &width, &height);
  /* Ai grijă la valoarea următoare. Nu știu de unde vine, dar este dependentă de sistemul de operare !!! */
#ifdef G_OS_WIN32
  height += 5;
#elif defined G_OS_UNIX
  height += 3;
#endif
  
  for (cnt = 0;  cnt < n_linii;  ++cnt) {
    if (y >= coly && y < (coly + height)) {
	  (*celly) = internalCellY;
      g_list_free(cells);
      return TRUE;
    }

    coly += height;
    internalCellY++;
  }

  g_list_free(cells);
  return FALSE; /* nu s-a găsit */
}

static void 
laEditareCelula(GtkCellRendererText *renderer, gchar *path, gchar *new_text, GtkTreeView *tv) {
  GtkTreeModel *trModel = NULL;
  GtkTreeIter trIter;
  gint colx = 0;
  gchar hexIntermediar[3];
  
  /* o scurtă validare a conținutului */
  if(strlen(new_text) == 0 || strlen(new_text) > 2 ||
     esteHexValid(new_text) == FALSE) {
    /* TODO: transformă acest eveniment într-un mesaj pentru utilizator */
    return;
  }
  
  /* aplică o mică corecție */
  g_sprintf(hexIntermediar, "%s%s", (strlen(new_text) == 1 ? "0" : ""), new_text);
  
  trModel = gtk_tree_view_get_model(tv);
  colx = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(renderer), "x-coord"));
  if(gtk_tree_model_get_iter_from_string(trModel, &trIter, path)) {
	gtk_list_store_set(GTK_LIST_STORE(trModel), &trIter, colx, hexIntermediar, -1);
  }
}

static gboolean 
esteHexValid(gchar *str) {
  gchar *p = NULL;
  
  p = str;
  while(*p != '\0') {
	if(g_ascii_isxdigit(*p) == FALSE) {
	  return FALSE;
	}
	*p = g_ascii_tolower(*p);
    p += sizeof(gchar);
  }
  
  return TRUE;
}

static void 
btCitesteEEPROM_click(GtkWidget *widget, FormularEEPROM *fe) {
  GtkTreeModel *tm = NULL;
  
  tm = gtk_tree_view_get_model(GTK_TREE_VIEW(fe->tvEEPROM));
  al_citeste_eeprom(GTK_LIST_STORE(tm));
}

static void 
btSalveazaEEPROM_click(GtkWidget *widget, FormularEEPROM *fe) {
  GtkTreeModel *tm = NULL;
  
  tm = gtk_tree_view_get_model(GTK_TREE_VIEW(fe->tvEEPROM));
  al_scrie_eeprom(GTK_LIST_STORE(tm));
}

static void 
btParasesteFrm_click(GtkWidget *widget, FormularEEPROM *fe) {
  gtk_widget_destroy(fe->frm);
}
