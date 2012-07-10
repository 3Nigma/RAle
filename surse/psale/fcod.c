#include "fcod.h"

#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourceview.h>

#define PSALE_CODS_IMPLICIT ".section text\n" \
  ".global main\n\n" \
  "main:\n" \
  "  ; introduceți codul dumneavoastră aici\n" \
  "ciclu_infinit:\n" \
  "  rjmp ciclu_infinit"
#define PSALE_CODC_IMPLICIT "#include \"ale.h\"\n\n" \
  "int main() {\n" \
  "  /* Scrieți codul dumneavoastră aici */\n"	\
  "  return 0;\n" \
  "}"

static GtkSourceLanguageManager *txtSrcLimbAsignor = NULL;

static gboolean 
frmCod_delev(GtkWidget *widget, GdkEvent *event, gpointer data) {
  return FALSE;
}

static void 
btExpandatorActiuni_click(GtkWidget *bt, FormularCod *fc) {
  GdkPixbuf *imgExpandatorPixBuf = NULL;
  GtkWidget *imgExpandatorActiuni = NULL;

  switch(fc->vActiuni) {
  case ASCUNSE:
    gtk_widget_show(fc->cadruActiuni);
    imgExpandatorPixBuf = gdk_pixbuf_new_from_file_at_size("media/bt_icoana_colapseaza.png", 16, 16, NULL);
    fc->vActiuni = VIZIBILE;
    break;
  case VIZIBILE:
    gtk_widget_hide(fc->cadruActiuni);
    imgExpandatorPixBuf = gdk_pixbuf_new_from_file_at_size("media/bt_icoana_expandeaza.png", 16, 16, NULL);
    fc->vActiuni = ASCUNSE;
    break;
  }

  imgExpandatorActiuni = gtk_image_new_from_pixbuf(imgExpandatorPixBuf);
  gtk_button_set_image(GTK_BUTTON(fc->btExpandator), imgExpandatorActiuni);
}

void 
fc_modifica_vizibilitate(FormularCod *fc, gboolean vizibil) {
  if(vizibil)  gtk_widget_show_all(fc->frm);
  else gtk_widget_hide_all(fc->frm);

  switch(fc->vActiuni) {
  case ASCUNSE:
    gtk_widget_hide(fc->cadruActiuni);
    break;
  case VIZIBILE:
    gtk_widget_show(fc->cadruActiuni);
    break;
  }
}

FormularCod *
fc_initializeaza(Limbaj lmDorit, const unsigned char *codInitial, gboolean esteExemplu) {
  FormularCod *deRet = NULL;
  GtkWidget *frm = NULL;
  GtkWidget *cadruFrm = NULL;
  GtkWidget *txtSrc = NULL;
  GtkWidget *btGestioneazaActiuni = NULL;
  GtkWidget *btIncarcaPeAle = NULL;
  GtkWidget *btSalveazaLucrul = NULL;
  GtkWidget *btReiaLucrul = NULL;
  GtkWidget *btCitesteEEPROM = NULL;
  GtkWidget *btParasesteFrm = NULL;
  GtkWidget *cadruBxActiuni = NULL;
  GtkWidget *cadruBxActiuniCentrale = NULL;
  GtkWidget *cadruBaraStare = NULL;
  GtkWidget *lblStareLegatura = NULL;
  GtkWidget *lblStareCod = NULL;
  GtkWidget *lblStareNumeSursa = NULL;

  /* inițializăm formularul principal */
  frm = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  switch(lmDorit) {
  case C:
    gtk_window_set_title(GTK_WINDOW(frm), "[C] psAle ~ Fereastră de cod");
    break;
  case ASM:
    gtk_window_set_title(GTK_WINDOW(frm), "[ASM] psAle ~ Fereastră de cod");
    break;
  }
  gtk_window_set_position(GTK_WINDOW(frm), GTK_WIN_POS_CENTER);

  /* inițializăm cadrul formularului de cod */
  cadruFrm = gtk_table_new(2, 3, FALSE);
  gtk_container_add(GTK_CONTAINER(frm), cadruFrm);

  /* inițializăm entități ajutătoare pentru elementul de cod */
  if(NULL == txtSrcLimbAsignor) {
    gchar *dirs[] = {"./limbaje", g_get_current_dir(), NULL};
    txtSrcLimbAsignor = gtk_source_language_manager_get_default();
    gtk_source_language_manager_set_search_path(txtSrcLimbAsignor, dirs);
  }
  GtkSourceLanguage *txtSrcLimb = NULL;
  switch(lmDorit) {
  case C:
    txtSrcLimb = gtk_source_language_manager_get_language(txtSrcLimbAsignor, "c");
    break;
  case ASM:
    txtSrcLimb = gtk_source_language_manager_get_language(txtSrcLimbAsignor, "asm");
    break;
  }
  GtkSourceBuffer *txtSrcBuf = gtk_source_buffer_new_with_language(txtSrcLimb);
  if(esteExemplu)
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtSrcBuf), codInitial, -1);
  else switch(lmDorit) {
    case C:
      gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtSrcBuf), PSALE_CODC_IMPLICIT, -1);
      break;
    case ASM:
      gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtSrcBuf), PSALE_CODS_IMPLICIT, -1);
      break;
    }

  /* inițializăm elementul central care va găzdui codul utilizatorului */
  txtSrc = gtk_source_view_new_with_buffer(txtSrcBuf);
  gtk_widget_set_size_request(txtSrc, 480, 320);
  gtk_source_view_set_tab_width(GTK_SOURCE_VIEW(txtSrc), 2);
  gtk_source_view_set_insert_spaces_instead_of_tabs(GTK_SOURCE_VIEW(txtSrc), TRUE);
  gtk_source_view_set_auto_indent(GTK_SOURCE_VIEW(txtSrc), TRUE);
  gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(txtSrc), TRUE);
  gtk_source_view_set_show_right_margin(GTK_SOURCE_VIEW(txtSrc), FALSE);

  GtkWidget *txtSrcScroll = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(txtSrcScroll), txtSrc);
  gtk_table_attach_defaults(GTK_TABLE(cadruFrm), txtSrcScroll, 0, 1, 0, 1);

  /* inițializăm butonul de afișare/ascundere acțiuni */
  btGestioneazaActiuni = gtk_button_new();
  GdkPixbuf *imgExpandatorPixBuf = gdk_pixbuf_new_from_file_at_size(esteExemplu ? "media/bt_icoana_colapseaza.png" : "media/bt_icoana_expandeaza.png", 
								    16, 16, NULL);
  GtkWidget *imgExpandatorActiuni = gtk_image_new_from_pixbuf(imgExpandatorPixBuf);
  gtk_button_set_image(GTK_BUTTON(btGestioneazaActiuni), imgExpandatorActiuni);
  gtk_table_attach(GTK_TABLE(cadruFrm), btGestioneazaActiuni, 1, 2, 0, 2, GTK_SHRINK, GTK_FILL, 0, 0);

  /* inițializează acțiunile speciale ale formularului de cod */  
  btIncarcaPeAle = gtk_button_new_with_label("Încarcă pe Ale");
  gtk_widget_set_size_request(GTK_WIDGET(btIncarcaPeAle), -1, 60);
  GtkWidget *imgIncarcaPeAle = gtk_image_new_from_pixbuf(gdk_pixbuf_new_from_file_at_size("media/bt_icoana_trimite_la_ale.png", 16, 16, NULL));
  gtk_button_set_image_position(GTK_BUTTON(btIncarcaPeAle), GTK_POS_RIGHT);
  gtk_button_set_image(GTK_BUTTON(btIncarcaPeAle), imgIncarcaPeAle);

  btSalveazaLucrul = gtk_button_new_with_label("Salvează lucrul");
  gtk_widget_set_size_request(GTK_WIDGET(btSalveazaLucrul), -1, 50);
  GtkWidget *imgSalveazaLucrul = gtk_image_new_from_pixbuf(gdk_pixbuf_new_from_file_at_size("media/bt_icoana_salveaza.png", 16, 16, NULL));
  gtk_button_set_image_position(GTK_BUTTON(btSalveazaLucrul), GTK_POS_LEFT);
  gtk_button_set_image(GTK_BUTTON(btSalveazaLucrul), imgSalveazaLucrul);

  btReiaLucrul = gtk_button_new_with_label("Reia cod");
  gtk_widget_set_size_request(GTK_WIDGET(btReiaLucrul), -1, 40);
  GtkWidget *imgReiaLucrul = gtk_image_new_from_pixbuf(gdk_pixbuf_new_from_file_at_size("media/bt_icoana_reia.png", 16, 16, NULL));
  gtk_button_set_image_position(GTK_BUTTON(btReiaLucrul), GTK_POS_LEFT);
  gtk_button_set_image(GTK_BUTTON(btReiaLucrul), imgReiaLucrul);

  btCitesteEEPROM = gtk_button_new_with_label("Citește EEPROM");
  gtk_widget_set_size_request(GTK_WIDGET(btCitesteEEPROM), -1, 35);
  GtkWidget *imgCitesteEEPROM = gtk_image_new_from_pixbuf(gdk_pixbuf_new_from_file_at_size("media/bt_icoana_eeprom.png", 16, 16, NULL));
  gtk_button_set_image_position(GTK_BUTTON(btCitesteEEPROM), GTK_POS_LEFT);
  gtk_button_set_image(GTK_BUTTON(btCitesteEEPROM), imgCitesteEEPROM);

  btParasesteFrm = gtk_button_new_with_label("Părăsește formular");
  gtk_widget_set_size_request(GTK_WIDGET(btParasesteFrm), -1, 30);
  GtkWidget *imgParasesteFrm = gtk_image_new_from_pixbuf(gdk_pixbuf_new_from_file_at_size("media/bt_icoana_paraseste.png", 16, 16, NULL));
  gtk_button_set_image_position(GTK_BUTTON(btParasesteFrm), GTK_POS_LEFT);
  gtk_button_set_image(GTK_BUTTON(btParasesteFrm), imgParasesteFrm);

  /* inițializăm cadrul regiunii de acțiuni */
  cadruBxActiuni = gtk_vbutton_box_new();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(cadruBxActiuni), GTK_BUTTONBOX_DEFAULT_STYLE);
  gtk_box_set_homogeneous(GTK_BOX(cadruBxActiuni), FALSE);
  cadruBxActiuniCentrale = gtk_vbutton_box_new();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(cadruBxActiuniCentrale), GTK_BUTTONBOX_CENTER);
  gtk_box_set_homogeneous(GTK_BOX(cadruBxActiuniCentrale), FALSE);

  gtk_box_pack_start(GTK_BOX(cadruBxActiuni), btIncarcaPeAle, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBxActiuniCentrale), btSalveazaLucrul, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBxActiuniCentrale), btReiaLucrul, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBxActiuniCentrale), btCitesteEEPROM, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBxActiuni), cadruBxActiuniCentrale, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBxActiuni), btParasesteFrm, FALSE, FALSE, 0);
  gtk_table_attach(GTK_TABLE(cadruFrm), cadruBxActiuni, 2, 3, 0, 2, GTK_SHRINK, GTK_FILL, 0, 0);

  /* inițializăm bara de stare a formularului */
  lblStareLegatura = gtk_label_new("[Conexiune]");
  lblStareCod = gtk_label_new("[Dim. Cod]");
  lblStareNumeSursa = gtk_label_new("[Nume Sursă]");

  cadruBaraStare = gtk_hbox_new(TRUE, 4);
  gtk_box_pack_start(GTK_BOX(cadruBaraStare), lblStareLegatura, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBaraStare), lblStareCod, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBaraStare), lblStareNumeSursa, FALSE, FALSE, 0);
  gtk_table_attach(GTK_TABLE(cadruFrm), cadruBaraStare, 0, 1, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  /* împachetăm elementele esențiale pentru a le returna */
  deRet = g_slice_new(FormularCod);
  deRet->frm = frm;
  deRet->btExpandator = btGestioneazaActiuni;
  deRet->cadruActiuni = cadruBxActiuni;
  deRet->lblStareConex = lblStareLegatura;
  deRet->lblStareDCod = lblStareCod;
  deRet->lblStareNSursa = lblStareNumeSursa;
  if(esteExemplu) deRet->vActiuni = VIZIBILE;
  else deRet->vActiuni = ASCUNSE;

  /* legăm semnalele de funcțiile recurente */
  g_signal_connect(frm, "delete-event", G_CALLBACK(frmCod_delev), NULL);
  g_signal_connect(btGestioneazaActiuni, "clicked", G_CALLBACK(btExpandatorActiuni_click), (gpointer)deRet);
  g_signal_connect_swapped(btParasesteFrm, "clicked", G_CALLBACK(gtk_widget_destroy), frm);

  return deRet;
}
