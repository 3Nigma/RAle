#include <stdlib.h>

#include <glib.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>

#ifdef G_OS_WIN32
#include <windows.h>
#endif

#include "db.h"
#include "finfo.h"
#include "fcod.h"

#ifndef G_VALUE_INIT 
  /* îl introducem aici pentru compilarea sub windows. Se pare că GTK-ul de windows nu-l definiește */
  #define G_VALUE_INIT { 0, { { 0 } } }
#endif

static GtkWidget *dlgInfo = NULL;

static gboolean 
imgLogo_click(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
#ifdef G_OS_WIN32
  ShellExecute(NULL, "open", "http://tuscale.ro", NULL, NULL, SW_SHOWNORMAL);
#elif defined G_OS_UNIX
  system("xdg-open 'http://tuscale.ro' &");
#endif
 
  return TRUE;
}

static void 
cmbxCodNou_selectat(GtkComboBox *widget, gpointer user_data) {
  /* interpretăm selecția doar dacă ea este una validă (se dorește un limbaj valid) */
  if(gtk_combo_box_get_active(widget) != 0) {
    FormularCod *dlgCod = NULL;

    switch(gtk_combo_box_get_active(widget)) {
    case 1: /* ASM */
      dlgCod = fc_initializeaza_fara_cod(ASM);
      break;
    case 2: /* C */
      dlgCod = fc_initializeaza_fara_cod(C);
      break;
    }

    /* repoziționăm textul afișat */
    gtk_combo_box_set_active(GTK_COMBO_BOX(widget), 0);

    fc_modifica_vizibilitate(dlgCod, TRUE);
  }
}

static void 
cmbxCodCarte_selectat(GtkComboBox *widget, gpointer user_data) {
  FormularCod *dlgCod = NULL;
  GtkTreeIter iter;
  GtkTreeModel *model = NULL;
  gchar *titluScurt = NULL;
  gchar *titluLung;
  gchar numeExempluAfisat[256];

  if(gtk_combo_box_get_active_iter(widget, &iter)) {
    model = gtk_combo_box_get_model(widget);
    gtk_tree_model_get(model, &iter, 0, &titluScurt, 1, &titluLung, -1);

    if(g_strcmp0(titluScurt, "") != 0) {
      g_sprintf(numeExempluAfisat, "Ex: %s", titluLung);
      if(g_str_has_suffix(titluScurt, "s")) dlgCod = fc_initializeaza(ASM, db_obtine_cod_complet(titluScurt), numeExempluAfisat, TRUE);
      else dlgCod = fc_initializeaza(C, db_obtine_cod_complet(titluScurt), numeExempluAfisat, TRUE);

      /* repoziționăm textul afișat */
      gtk_combo_box_set_active(GTK_COMBO_BOX(widget), 0);

      fc_modifica_vizibilitate(dlgCod, TRUE);
    }
  }
}

static void 
btInfo_click(GtkWidget *widget, gpointer data) {
  if(dlgInfo == NULL)
    dlgInfo = initializeaza_formular_info();

  gtk_widget_show_all(dlgInfo);
}

static gboolean 
frmPrincipal_delev(GtkWidget *widget, GdkEvent *event, gpointer data) {

  return FALSE;
}

static void 
frmPrincipal_destroy(GtkWidget *widget, gpointer data) {
  gtk_main_quit();
}

static GtkWidget *
creeaza_cmbxExemple() {
  GtkWidget *deRet = NULL;
  GtkListStore *magazie = NULL;
  GtkCellRenderer *afisorCel0 = NULL;
  GtkCellRenderer *afisorCel1 = NULL;
  GtkTreeIter iter;
  GValue val = G_VALUE_INIT;

  g_value_init(&val, G_TYPE_STRING);

  magazie = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
  gtk_list_store_append(magazie, &iter);
  gtk_list_store_set(magazie, &iter, 0, "", 1, "Cod din cărțulie ...", -1);
  db_incarca_exemple_carte(magazie);

  deRet = gtk_combo_box_new_with_model(GTK_TREE_MODEL(magazie));
  //gtk_widget_set_size_request(GTK_WIDGET(deRet), 80, -1);
  gtk_combo_box_set_active(GTK_COMBO_BOX(deRet), 0);
  g_object_unref(G_OBJECT(magazie));

  afisorCel0 = gtk_cell_renderer_text_new();
  g_value_set_static_string (&val, "Sans Italic 8");
  g_object_set_property(G_OBJECT(afisorCel0), "font", &val);
  afisorCel1 = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(deRet), afisorCel0, TRUE);
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(deRet), afisorCel1, TRUE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(deRet), afisorCel0, "text", 0, NULL);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(deRet), afisorCel1, "text", 1, NULL);

  return deRet;
}

int main(int argc, char *argv[]) {
  GtkWidget *cadruFormPrincipal = NULL;
  GtkWidget *formPrincipal = NULL;
  GtkWidget *cadruImgLogo = NULL;
  GtkWidget *imgLogo = NULL;
  GtkWidget *cmbxCodNou = NULL;
  GtkWidget *cmbxCodCarte = NULL;
  GtkWidget *btInfo = NULL;
  GtkWidget *btIesire = NULL;

  gtk_init (&argc, &argv);
  /* Dezactivate momentan pentru a nu folosi inutil resursele rețelei
     dl_initializeaza(db_obtine_adresa_actualizare());

     dl_actualizeaza_bd();*/

  /* inițializăm formularul principal */
  formPrincipal = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width(GTK_CONTAINER(formPrincipal), 8);
  gtk_window_set_resizable(GTK_WINDOW(formPrincipal), FALSE);
  gtk_window_set_title(GTK_WINDOW(formPrincipal), "psAle");
  gtk_window_set_position(GTK_WINDOW(formPrincipal), GTK_WIN_POS_CENTER);
  g_signal_connect(formPrincipal, "delete-event", G_CALLBACK(frmPrincipal_delev), NULL);
  g_signal_connect(formPrincipal, "destroy", G_CALLBACK(frmPrincipal_destroy), NULL);

  /* inițializăm cadrul formularului principal */
  cadruFormPrincipal = gtk_vbox_new(FALSE, 5);
  gtk_container_add(GTK_CONTAINER(formPrincipal), cadruFormPrincipal);

  /* inițializăm logoul aplicației */
  imgLogo = gtk_image_new_from_file("media/tuscale_small_logo_ale.png");

  /* inițializăm cadrul pentru logoul aplicației */
  cadruImgLogo = gtk_event_box_new();
  gtk_widget_set_events(cadruImgLogo, GDK_KEY_PRESS_MASK);
  gtk_container_add(GTK_CONTAINER(cadruImgLogo), imgLogo);
  gtk_container_add(GTK_CONTAINER(cadruFormPrincipal), cadruImgLogo);
  g_signal_connect(cadruImgLogo, "button-press-event", G_CALLBACK(imgLogo_click), FALSE);

  /* inițializăm meniul de selecție pentru programare brută */
  cmbxCodNou = gtk_combo_box_text_new();
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbxCodNou), "Hai la programare ...");
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbxCodNou), "... în ASM");
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbxCodNou), "... în C");
  gtk_combo_box_set_active(GTK_COMBO_BOX(cmbxCodNou), 0);
  g_signal_connect(cmbxCodNou, "changed", G_CALLBACK(cmbxCodNou_selectat), NULL);
  gtk_container_add(GTK_CONTAINER(cadruFormPrincipal), cmbxCodNou);

  /* inițializăm meniul de selecție pentru exemplele de cod din carte */
  cmbxCodCarte = creeaza_cmbxExemple();
  g_signal_connect(cmbxCodCarte, "changed", G_CALLBACK(cmbxCodCarte_selectat), NULL);
  gtk_container_add(GTK_CONTAINER(cadruFormPrincipal), cmbxCodCarte);

  /* inițializăm butonul de informații */
  btInfo = gtk_button_new_with_label("Ce avem aici?");
  g_signal_connect(btInfo, "clicked", G_CALLBACK(btInfo_click), NULL);
  gtk_container_add(GTK_CONTAINER(cadruFormPrincipal), btInfo);

  /* inițializăm butonul de ieșire */
  btIesire = gtk_button_new_with_label("Gata, am ieșit!");
  g_signal_connect_swapped(btIesire, "clicked", G_CALLBACK(gtk_widget_destroy), formPrincipal);
  gtk_container_add(GTK_CONTAINER(cadruFormPrincipal), btIesire);

  gtk_widget_show_all(formPrincipal);

  /* permite butoanelor să afișeze atât imaginea cât și textul dorit */
  GtkSettings *default_settings = gtk_settings_get_default();
  g_object_set(default_settings, "gtk-button-images", TRUE, NULL); 
  gtk_main();
  dl_curata();

  return 0;
}
